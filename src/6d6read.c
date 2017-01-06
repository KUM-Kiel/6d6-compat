#define _FILE_OFFSET_BITS 64
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "6d6.h"
#include "bcd.h"
#include "number.h"
#include "options.h"
#include "s2x.h"
#include "s2x_channel.h"
#include "version.h"

static const char *program = "6d6read";
static void help(const char *arg)
{
  fprintf(stderr, "Version %s (%s)\n",
    KUM_6D6_COMPAT_VERSION, KUM_6D6_COMPAT_DATE);
  fprintf(stderr,
    "Usage: %s [-q|--no-progress] < in.6d6 > out.s2x\n"
    "\n"
    "The program '6d6read' is used to convert raw data from th 6D6 datalogger\n"
    "into the Send2X format.\n"
    "This is useful if you still have a lot of old dataloggers and want to use\n"
    "a uniform method of data analysis.\n"
    "\n"
    "The input file is a .6d6 file and is written with an opening angle bracket '<'.\n"
    "The output file will be a .s2x file and is preceded by a closing angle\n"
    "bracket '>'.\n"
    "\n"
    "If the input file is a valid .6d6 file, the conversion process will begin and\n"
    "the progress will be reported to the terminal. If you want to suppress the\n"
    "progress display, you can use the flag '-q' or '--no-progress'. This might be\n"
    "useful in automated scripts.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "\n"
    "Convert the file 'station-007.6d6' to Send2X format:\n"
    "\n"
    "  $ 6d6read < station-007.6d6 > station-007.s2x\n"
    "\n"
    "Convert the file 'x.6d6' to 'y.s2x' and suppress the progress display:\n"
    "\n"
    "  $ 6d6read --no-progress < x.6d6 > y.s2x\n",
    program);
  exit(1);
}

static void read_block(uint8_t *block, FILE *f)
{
  if (fread(block, 512, 1, f) != 1) {
    fprintf(stderr, "I/O error\n");
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

int main(int argc, char **argv)
{
  s2x_header h;
  kum_6d6_header h_start, h_end;
  FILE *input = stdin, *output = stdout;
  s2x_channel *channels[KUM_6D6_MAX_CHANNEL_COUNT];
  uint8_t block[512], x[16];
  char str[512];
  uint32_t i, j;
  int c, e;
  int64_t start_time, sync_time, skew_time = 0;
  int32_t skew = 0;
  /* Block parser. */
  int pos, remaining = 0, have_time = 0;
  int32_t frame[KUM_6D6_MAX_CHANNEL_COUNT];

  int progress = 1;
  program = argv[0];
  parse_options(&argc, &argv, OPTIONS(
    FLAG('p', "progress", progress, 1),
    FLAG('q', "no-progress", progress, 0),
    FLAG_CALLBACK('h', "help", help)
  ));

  /* Set input/output files. */
  if (isatty(0)) {
    if (argc == 2) {
      input = fopen(argv[1], "rb");
      if (!input) {
        e = errno;
        snprintf(str, sizeof(str), "/dev/%s", argv[1]);
        input = fopen(str, "rb");
        if (!input) {
          fprintf(stderr, "Could not open '%s': %s.\n", argv[1], strerror(e));
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
  setuid(getuid());

  read_block(block, input);
  if (kum_6d6_header_read(&h_start, block) == -1) {
    read_block(block, input);
    if (kum_6d6_header_read(&h_start, block) == -1) {
      fprintf(stderr, "Malformed 6D6 start header\n");
      exit(1);
    }
  }
  read_block(block, input);
  if (kum_6d6_header_read(&h_end, block) == -1) {
    fprintf(stderr, "Malformed 6D6 end header\n");
    exit(1);
  }

  /* Calculate times. */
  start_time = bcd2gps(h_start.start_time);
  sync_time = bcd2gps(h_start.sync_time);
  if (h_end.sync_type == KUM_6D6_SKEW) {
    skew_time = bcd2gps(h_end.sync_time);
    skew = round(h_end.skew / 1000.0);
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
            if ((start_time + frame[1]) % 60 == 0) {
              have_time = 1;
              for (c = 0; c < h_start.channel_count; ++c) {
                s2x_channel_flush(channels[c]);
              }
              st_i32_le(x, S2X_TIME);
              st_i32_le(x + 4, 4);
              st_i32_le(x + 8, -1);
              st_i32_le(x + 12, start_time + frame[1]);
              if (fwrite(x, 16, 1, output) != 1) {
                fprintf(stderr, "I/O error\n");
                return 1;
              }
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
            for (c = 0; c < h_start.channel_count; ++c) {
              s2x_channel_flush(channels[c]);
            }
            st_i32_le(x, S2X_TERMINATE);
            st_i32_le(x + 4, 0);
            st_i32_le(x + 8, -1);
            if (fwrite(x, 12, 1, output) != 1) {
              fprintf(stderr, "I/O error\n");
              return 1;
            }
            goto done;
          case 15: /* Frame Number */
            break;
          default: break;
          }
        } else {
          if (have_time) {
            for (c = 0; c < h_start.channel_count; ++c) {
              s2x_channel_push(channels[c], frame[c]);
            }
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
  if (progress) {
    fprintf(stderr, "%3d%% %6.1fMB     \n", 100, (double) h_end.address * 512 / 1000000l);
    fflush(stderr);
  }

  return 0;
}
