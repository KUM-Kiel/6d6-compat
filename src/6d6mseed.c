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
#include "version.h"
#define TAI_IMPLEMENTATION
#include "tai.h"
#define MINISEED_IMPLEMENTATION
#include "miniseed.h"
#define SAMPLEBUFFER_IMPLEMENTATION
#include "samplebuffer.h"
#define WMSEED_IMPLEMENTATION
#include "wmseed.h"

static const char *program = "6d6mseed";
static void help(const char *arg)
{
  fprintf(stderr, "Version %s (%s)\n",
    KUM_6D6_COMPAT_VERSION, KUM_6D6_COMPAT_DATE);
  fprintf(stderr,
    "Usage: %s [options] input.6d6\n"
    "\n"
    "The program '6d6mseed' is used to convert raw data from the 6D6 datalogger\n"
    "into the MiniSEED format.\n",
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

int main(int argc, char **argv)
{
  kum_6d6_header h_start, h_end;
  FILE *input = 0;
  WMSeed *channels[KUM_6D6_MAX_CHANNEL_COUNT];
  uint8_t block[512];
  char str[512];
  uint32_t i, j;
  int c, e;
  Time start_time, sync_time, skew_time = 0, t;
  /* Block parser. */
  int pos, remaining = 0, have_time = 0;
  int32_t frame[KUM_6D6_MAX_CHANNEL_COUNT];
  int64_t sample_number = 0;

  char *station = 0, *location = "", *network = "";
  char *template = "out/%S/%y-%m-%d-%C.mseed";
  int cut = 86400;

  int progress = 1;
  program = argv[0];
  parse_options(&argc, &argv, OPTIONS(
    FLAG('p', "progress", progress, 1),
    FLAG('q', "no-progress", progress, 0),
    FLAG_CALLBACK('h', "help", help),
    PARAMETER(0, "station", station),
    PARAMETER(0, "location", location),
    PARAMETER(0, "network", network),
    PARAMETER(0, "output", template)
  ));

  /* Set input file. */
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
  start_time = bcd_time(h_start.start_time);
  sync_time = bcd_time(h_start.sync_time);
  if (h_end.sync_type == KUM_6D6_SKEW) {
    skew_time = bcd_time(h_end.sync_time);
  }
  /* Create Channels. */
  for (c = 0; c < h_start.channel_count; ++c) {
    channels[c] = wmseed_new(
      template,
      station, location, (const char *) h_start.channel_names[c], network,
      h_start.sample_rate,
      cut);
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
            have_time = 1;
            for (c = 0; c < h_start.channel_count; ++c) {
              t = start_time + (frame[1] * (int64_t) 1000000);
              if (h_end.sync_type == KUM_6D6_SKEW) {
                t += h_start.skew + round((double) (t - sync_time) * (h_end.skew - h_start.skew) / (skew_time - sync_time));
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
            for (c = 0; c < h_start.channel_count; ++c) {
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
  for (c = 0; c < h_start.channel_count; ++c) {
    wmseed_destroy(channels[c]);
  }
  if (progress) {
    fprintf(stderr, "%3d%% %6.1fMB     \n", 100, (double) h_end.address * 512 / 1000000l);
    fflush(stderr);
  }

  return 0;
}
