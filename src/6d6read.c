#define _FILE_OFFSET_BITS 64
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include "6d6.h"
#include "bcd.h"
#include "number.h"
#include "options.h"
#include "s2x.h"
#include "s2x_channel.h"
#include "version.h"
#include "i18n.h"
#include "i18n_error.h"
#include "monotonic-time.h"
#include "tai.h"

#define SAMPLE_TRACKER_IMPLEMENTATION
#include "sample-tracker.h"

static const char *program = "6d6read";
static void help(const char *arg)
{
  fprintf(stdout, i18n->version_ss,
    KUM_6D6_COMPAT_VERSION, KUM_6D6_COMPAT_DATE);
  fprintf(stdout, i18n->usage_6d6read_s,
    program);
  exit(1);
}

static void read_block(uint8_t *block, FILE *f)
{
  if (fread(block, 512, 1, f) != 1) {
    fprintf(stderr, "%s", i18n->io_error);
    exit(1);
  }
}

static void copy_string(void *dst, const void *src, int l)
{
  char *d = dst;
  const char *s = src;
  if (!s || !d) return;
  while (l > 1 && *s) {
    *(d++) = *(s++);
    --l;
  }
  while (l > 0) {
    *(d++) = 0;
    --l;
  }
}

static void copy_string_space(void *dst, const void *src, int l)
{
  char *d = dst;
  const char *s = src;
  if (!s || !d) return;
  while (l > 1 && *s) {
    *(d++) = *(s++);
    --l;
  }
  while (l > 0) {
    *(d++) = ' ';
    --l;
  }
}

static int64_t bcd2gps(void *bcd)
{
  static const char y2k[6] = {
    0, 0, 0, 1, 1, 0
  };
  return bcd_diff(y2k, bcd) + 630720000;
}

static const char *mcs_channels[] = {
  "Hydrophone H",
  "Seismometer X",
  "Seismometer Y",
  "Seismometer Z"
};

static Time bcd_time(const uint8_t *bcd)
{
  Date date = {
    .year = bcd_int(bcd[BCD_YEAR]) + 2000,
    .month = bcd_int(bcd[BCD_MONTH]),
    .day = bcd_int(bcd[BCD_DAY]),
    .hour = bcd_int(bcd[BCD_HOUR]),
    .min = bcd_int(bcd[BCD_MINUTE]),
    .sec = bcd_int(bcd[BCD_SECOND]),
    .usec = 0
  };
  return tai_time(date);
}

static int parse_time(const char *s, Time *t)
{
  Date d;
  if (!(s = tai_parse_date(s, &d))) return -1;
  while (*s == ' ') {
    ++s;
  }
  if (*s) return -1;
  if (t) *t = tai_time(d);
  return 0;
}

int main(int argc, char **argv)
{
  s2x_header h;
  kum_6d6_header h_start, h_end;
  FILE *input = stdin, *output = stdout;
  SampleTracker st[1];
  s2x_channel *channels[KUM_6D6_MAX_CHANNEL_COUNT];
  uint8_t block[512], x[16];
  char str[512];
  uint32_t i, j;
  int c, e;
  int64_t start_time, sync_time, skew_time = 0;
  int64_t t, last_t = -1;
  int32_t skew = 0;
  /* Block parser. */
  int pos, remaining = 0, have_time = 0;
  int32_t frame[KUM_6D6_MAX_CHANNEL_COUNT];

  Time start_time_tai, time_tai;
  Time start_time_limit = INT64_MIN;
  Time end_time_limit = INT64_MAX;
  char *start_time_string = 0;
  char *end_time_string = 0;

  i18n_set_lang(getenv("LANG"));

  int progress = 1;

  int64_t t0, t1, t2, _50ms, total;

  program = argv[0];
  parse_options(&argc, &argv, OPTIONS(
    FLAG('p', "progress", progress, 1),
    FLAG('q', "no-progress", progress, 0),
    FLAG(0, "json-progress", progress, 2),
    PARAMETER(0, "start-time", start_time_string),
    PARAMETER(0, "end-time", end_time_string),
    FLAG_CALLBACK('h', "help", help)
  ));

  if (start_time_string) {
    if (parse_time(start_time_string, &start_time_limit)) {
      fprintf(stderr, "%s", i18n->invalid_start_time);
      exit(1);
    }
  }

  if (end_time_string) {
    if (parse_time(end_time_string, &end_time_limit)) {
      fprintf(stderr, "%s", i18n->invalid_end_time);
      exit(1);
    }
  }

  /* Set input/output files. */
  if (isatty(0)) {
    if (argc == 2) {
      input = fopen(argv[1], "rb");
      if (!input) {
        e = errno;
        snprintf(str, sizeof(str), "/dev/%s", argv[1]);
        input = fopen(str, "rb");
        if (!input) {
          fprintf(stderr, i18n->could_not_open_ss, argv[1], i18n_error(e));
          exit(1);
        }
      }
    } else {
      help(0);
    }
  }

  if (isatty(1)) {
    help(0);
  }

  /* Drop root privileges if we had any. */
  uid_t uid = getuid();
  if (uid > 0 && setuid(uid) < 0) {
    fprintf(stderr, "%s", i18n->could_not_restore_uid);
    exit(1);
  }

  read_block(block, input);
  if (kum_6d6_header_read(&h_start, block) == -1) {
    read_block(block, input);
    if (kum_6d6_header_read(&h_start, block) == -1) {
      fprintf(stderr, "%s", i18n->malformed_6d6_header);
      exit(1);
    }
  }
  read_block(block, input);
  if (kum_6d6_header_read(&h_end, block) == -1) {
    fprintf(stderr, "%s", i18n->malformed_6d6_header);
    exit(1);
  }

  /* Init sample tracker. */
  sample_tracker_init(st, h_start.sample_rate);

  /* Calculate times. */
  start_time_tai = bcd_time(h_start.start_time);
  start_time = bcd2gps(h_start.start_time);
  sync_time = bcd2gps(h_start.sync_time);
  if (h_end.sync_type == KUM_6D6_SKEW) {
    skew_time = bcd2gps(h_end.sync_time);
    //skew = round(h_end.skew / 1000.0);
    /* It seems the manual is wrong and skew is in fact in us. */
    /* Also the 6D6 skew and the Send skew have opposite signs. */
    skew = -h_end.skew;
  }
  /* Create Channels. */
  for (c = 0; c < h_start.channel_count; ++c) {
    s2x_header_init(&h);
    h.version = 0x100;
    h.channel = c;
    //copy_string_space(h.chan_name, h_start.channel_names[c], sizeof(h.chan_name));
    copy_string_space(h.chan_name, mcs_channels[c], sizeof(h.chan_name));
    copy_string_space(h.sensor_name, "", sizeof(h.sensor_name));
    copy_string_space(h.sensor_nr, "", sizeof(h.sensor_nr));
    copy_string_space(h.chan_comm, "", sizeof(h.chan_comm));
    h.channel_sensitivity = 0x4a000000;
    copy_string(h.das_model, "KUM 6D6", sizeof(h.das_model));
    h.ser_nr = atoi((const char *) h_start.recorder_id);
    h.gain = ceil(h_start.gain[c] / 10.0);
    h.start_time = start_time;
    h.sync_time = sync_time;
    h.skew_time = skew_time;
    h.skew = skew;
    h.sample_period = ceil(1000000.0 / h_start.sample_rate);
    h.used_bits = 32;
    copy_string_space(h.exp_name, "", sizeof(h.exp_name));
    copy_string_space(h.exp_comm, "", sizeof(h.exp_comm));
    copy_string_space(h.station_name, "", sizeof(h.station_name));
    copy_string_space(h.station_comm, "", sizeof(h.station_comm));
    channels[c] = s2x_channel_new(&h, output);
  }

  _50ms = monotonic_time_ms(50);
  t0 = t1 = monotonic_time();
  total = h_end.address * 512;
  if (progress == 2) {
    fprintf(stderr, "{\"done\":0,\"total\":%"PRId64",\"elapsed\":0}\n", total);
    fflush(stderr);
  }

  i = 2;
  /* Skip to start of data.
   * Can not fseek, because stream might not be seekable. */
  while (i < h_start.address) {
    read_block(block, input);
    ++i;
  }
  /* Read data. */
  while (i < h_end.address) {
    read_block(block, input);
    ++i;
    /* Process block. */
    for (j = 0; j < 512; j += 4) {
      if (!remaining) {
        frame[0] = ld_i32_be(block + j);
        remaining = (frame[0] & 1) ? 3 : (h_start.channel_count - 1);
        pos = 1;
      } else {
        frame[pos++] = ld_i32_be(block + j);
        --remaining;
      }
      if (!remaining) {
        if (frame[0] & 1) {
          /* Control Frame */
          switch (frame[0]) {
          case 1: /* Time */
            /* Put the time into the sample tracker. */
            sample_tracker_time(st, (int64_t) frame[1] * 1000000 + frame[2]);
            if (!have_time) have_time = 1;
            break;
          case 3: /* VBat/Humidity */
            break;
          case 5: /* Temperature */
            break;
          case 7: /* Lost Frames */
            break;
          case 9: /* Check Frame */
            break;
          case 11: /* Reboot */
            break;
          case 13: /* End Frame */
            goto done;
          case 15: /* Frame Number */
            break;
          default: break;
          }
        } else {
          if (have_time) {
            t = sample_tracker_sample(st);
            time_tai = start_time_tai + t;
            if (start_time_limit <= time_tai && time_tai < end_time_limit) {
              if (last_t >= 0 && t / 1000000 != last_t / 1000000) {
                for (c = 0; c < h_start.channel_count; ++c) {
                  s2x_channel_flush(channels[c]);
                }
                st_i32_le(x, S2X_TIME);
                st_i32_le(x + 4, 4);
                st_i32_le(x + 8, -1);
                st_i32_le(x + 12, start_time + t / 1000000);
                if (fwrite(x, 16, 1, output) != 1) {
                  fprintf(stderr, "%s", i18n->io_error);
                  return 1;
                }
                have_time = 2;
              }
              last_t = t;
            }
          }
          if (have_time == 2) {
            for (c = 0; c < h_start.channel_count; ++c) {
              s2x_channel_push(channels[c], frame[c]);
            }
          }
        }
      }
    }
    if (progress == 1 && i % 1024 == 0) {
      fprintf(stderr, "%3d%% %6.1fMB     \r", (int) (i * 100 / h_end.address), (double) i * 512 / 1000000l);
      fflush(stderr);
    } else if (progress == 2) {
      t2 = monotonic_time();
      if (t2 - t1 >= _50ms) {
        fprintf(stderr,
          "{\"done\":%"PRId64",\"total\":%"PRId64",\"elapsed\":%"PRId64"}\n",
          (int64_t) i * 512, total, (t2 - t0) * 50 / _50ms);
        fflush(stderr);
        t1 = t2;
      }
    }
  }
done:
  for (c = 0; c < h_start.channel_count; ++c) {
    s2x_channel_flush(channels[c]);
  }
  st_i32_le(x, S2X_TERMINATE);
  st_i32_le(x + 4, 0);
  st_i32_le(x + 8, -1);
  if (fwrite(x, 12, 1, output) != 1) {
    fprintf(stderr, "%s", i18n->io_error);
    return 1;
  }
  if (progress == 1) {
    fprintf(stderr, "%3d%% %6.1fMB     \n", 100, (double) h_end.address * 512 / 1000000l);
    fflush(stderr);
  } else if (progress == 2) {
    t2 = monotonic_time();
    fprintf(stderr,
          "{\"done\":%"PRId64",\"total\":%"PRId64",\"elapsed\":%"PRId64"}\n",
          total, total, (t2 - t0) * 50 / _50ms);
    fflush(stderr);
  }

  return 0;
}
