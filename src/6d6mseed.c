#define _FILE_OFFSET_BITS 64
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include "6d6.h"
#include "bcd.h"
#include "number.h"
#include "options.h"
#include "version.h"
#include "tai.h"
#include "i18n.h"
#include "i18n_error.h"
#define MINISEED_IMPLEMENTATION
#include "miniseed.h"
#define SAMPLEBUFFER_IMPLEMENTATION
#include "samplebuffer.h"
#define WMSEED_IMPLEMENTATION
#include "wmseed.h"

static FILE *_logfile = 0;
static void log_entry(FILE *f, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  if (_logfile) {
    vfprintf(_logfile, format, args);
    fflush(_logfile);
  }
  vfprintf(f, format, args);
  fflush(f);
  va_end(args);
}

static void fatal(const char *s)
{
  log_entry(stderr, "%s", s);
  exit(1);
}

static const char *program = "6d6mseed";
static void help(const char *arg)
{
  fprintf(stdout, i18n->version_ss,
    KUM_6D6_COMPAT_VERSION, KUM_6D6_COMPAT_DATE);
  fprintf(stdout, i18n->usage_6d6mseed_s, program);
  exit(1);
}

static void read_block(uint8_t *block, FILE *f)
{
  if (fread(block, 512, 1, f) != 1) {
    log_entry(stderr, i18n->io_error);
    exit(1);
  }
}

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

static int alphanum(const char *s)
{
  while (*s) {
    if ((*s < '0' || *s > '9') &&
        (*s < 'A' || *s > 'Z') &&
        (*s < 'a' || *s > 'z')) {
      return 0;
    }
    ++s;
  }
  return 1;
}

int main(int argc, char **argv)
{
  kum_6d6_header h_start, h_end;
  FILE *input = 0;
  WMSeed *channels[KUM_6D6_MAX_CHANNEL_COUNT];
  int n_channels;
  uint8_t block[512];
  char str[512];
  const char *filename = "-";
  uint32_t i, j;
  int c, e;
  Time start_time, sync_time, skew_time = 0, t;
  double skew = 1;
  int have_skew = 0;
  /* Block parser. */
  int pos, remaining = 0, have_time = 0;
  int32_t frame[KUM_6D6_MAX_CHANNEL_COUNT];
  int64_t sample_number = 0;

  char *station = 0, *location = "", *network = "";
  char *template = 0;
  char *cut_string = 0;
  char *logfile = 0;
  int cut = 86400;

  int progress = 1;

  i18n_set_lang(getenv("LANG"));

  int outdated = tai_leapsecs_need_update(tai_now());
  if (outdated) {
    fprintf(stderr, "%s", i18n->leapsec_outdated);
  }

  program = argv[0];
  parse_options(&argc, &argv, OPTIONS(
    FLAG('p', "progress", progress, 1),
    FLAG('q', "no-progress", progress, 0),
    FLAG_CALLBACK('h', "help", help),
    PARAMETER(0, "station", station),
    PARAMETER(0, "location", location),
    PARAMETER(0, "network", network),
    PARAMETER(0, "output", template),
    PARAMETER('c', "cut", cut_string),
    PARAMETER('l', "logfile", logfile)
  ));

  if (cut_string) {
    int n = 0;
    while (*cut_string) {
      if (*cut_string < '0' || *cut_string > '9') fatal(i18n->invalid_cut);
      if (__builtin_mul_overflow(n, 10, &n)) fatal(i18n->invalid_cut);
      if (__builtin_add_overflow(n, *cut_string - '0', &n)) fatal(i18n->invalid_cut);
      ++cut_string;
    }
    if (n < 300 || n > 86400 * 60) fatal(i18n->invalid_cut);
    cut = n;
  }

  if (!template) {
    if (cut >= 86400) {
      template = "out/%S/%y-%m-%d-%C.mseed";
    } else {
      template = "out/%S/%y-%m-%d_%h.%i.%s-%C.mseed";
    }
  }

  /* Set input file. */
  if (argc == 2) {
    filename = argv[1];
    input = fopen(filename, "rb");
    if (!input) {
      e = errno;
      snprintf(str, sizeof(str), "/dev/%s", argv[1]);
      filename = str;
      input = fopen(filename, "rb");
      if (!input) {
        fprintf(stderr, i18n->could_not_open_ss, argv[1], i18n_error(e));
        exit(1);
      }
    }
  } else {
    help(0);
  }

  /* Drop root privileges if we had any. */
  (void) setuid(getuid());

  /* Create the logfile. */
  if (logfile) {
    _logfile = fopen(logfile, "wb");
    if (!_logfile) {
      fprintf(stderr, i18n->could_not_open_logfile_s, i18n_error(errno));
    }
  }

  /* Check the leapsecond information. */
  if (outdated && _logfile) {
    fprintf(_logfile, "%s", i18n->leapsec_outdated);
  }

  if (!station || strlen(station) <= 0 || strlen(station) > 5 || !alphanum(station)) {
    fatal(i18n->invalid_station_code);
  }

  log_entry(stderr, i18n->processing_s, filename);
  log_entry(stderr, "============================================================\n");

  read_block(block, input);
  if (kum_6d6_header_read(&h_start, block) == -1) {
    read_block(block, input);
    if (kum_6d6_header_read(&h_start, block) == -1) {
      log_entry(stderr, "%s", i18n->malformed_6d6_header);
      exit(1);
    }
  }
  read_block(block, input);
  if (kum_6d6_header_read(&h_end, block) == -1) {
    log_entry(stderr, "%s", i18n->malformed_6d6_header);
    exit(1);
  }

  /* Calculate times. */
  sync_time = bcd_time(h_start.sync_time);
  start_time = bcd_time(h_start.start_time);
  /* Leap second between sync and start? */
  start_time += 1000000 * (tai_utc_diff(start_time) - tai_utc_diff(sync_time));
  if (h_end.sync_type == KUM_6D6_SKEW && bcd_valid((const char *) h_end.sync_time)) {
    skew_time = bcd_time(h_end.sync_time);
    h_end.skew += 1000000 * (tai_utc_diff(skew_time) - tai_utc_diff(sync_time));
    skew = (double) (h_end.skew - h_start.skew) / (skew_time - sync_time);
    have_skew = 1;
  }
  /* Create Channels. */
  n_channels = h_start.channel_count;
  assert(0 <= n_channels && n_channels <= KUM_6D6_MAX_CHANNEL_COUNT);
  for (c = 0; c < n_channels; ++c) {
    channels[c] = wmseed_new(
      _logfile,
      template,
      station, location, (const char *) h_start.channel_names[c], network,
      h_start.sample_rate,
      cut);
  }

  if (_logfile) {
    kum_6d6_show_info(_logfile, &h_start, &h_end);
  }
  kum_6d6_show_info(stderr, &h_start, &h_end);

  log_entry(stderr, "============================================================\n");

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
        remaining = (frame[0] & 1) ? 3 : (n_channels - 1);
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
            have_time = 1;
            for (c = 0; c < n_channels; ++c) {
              t = start_time + (frame[1] * (int64_t) 1000000);
              if (have_skew) {
                t += h_start.skew + round((t - sync_time) * skew);
              }
              wmseed_time(channels[c], t, sample_number);
            }
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
            for (c = 0; c < n_channels; ++c) {
              wmseed_sample(channels[c], frame[c]);
            }
            sample_number += 1;
          }
        }
      }
    }
    if (progress && i % 1024 == 0) {
      fprintf(stderr, "%3d%% %6.1fMB     \r", (int) (i * 100 / h_end.address), (double) i * 512 / 1000000l);
      fflush(stderr);
    }
  }
done:
  for (c = 0; c < n_channels; ++c) {
    wmseed_destroy(channels[c]);
  }
  if (progress) {
    log_entry(stderr, "%3d%% %6.1fMB     \n", 100, (double) h_end.address * 512 / 1000000l);
    fflush(stderr);
  }

  return 0;
}
