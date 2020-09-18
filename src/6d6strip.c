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

static const char *program = "6d6strip";
static void help(const char *arg)
{
  fprintf(stdout, i18n->version_ss,
    KUM_6D6_COMPAT_VERSION, KUM_6D6_COMPAT_DATE);
  fprintf(stdout, "Usage: %s input.6d6 output.6d6\n", program);
  exit(1);
}

static void read_block(uint8_t *block, FILE *f)
{
  if (fread(block, 512, 1, f) != 1) {
    fprintf(stderr, "%s", i18n->io_error);
    exit(1);
  }
}

static int try_read_block(uint8_t *block, FILE *f)
{
  if (fread(block, 512, 1, f) != 1) {
    fprintf(stderr, "%s", i18n->io_error);
    return 1;
  }
  return 0;
}

static void write_or_fail(FILE *f, void *data, size_t len) {
  if (fwrite(data, len, 1, f) != 1) {
    fprintf(stderr, "%s", i18n->io_error);
    exit(1);
  }
}

static uint64_t zigzag_encode(int64_t n)
{
  return (n << 1) ^ (n >> (8 * sizeof(n) - 1));
}

#if 1
static void write_varint(FILE *f, uint64_t i)
{
  uint8_t buf[10];
  size_t n = 0;
  while (i > 127) {
    buf[n++] = (i & 127) | 128;
    i >>= 7;
  }
  buf[n++] = i & 127;
  write_or_fail(f, buf, n);
}
#else
static void write_varint(FILE *f, uint64_t i)
{
  write_or_fail(f, &i, 8);
}
#endif

int main(int argc, char **argv)
{
  kum_6d6_header h_start, h_end;
  FILE *input = 0, *output = 0;
  int n_channels;
  uint8_t block[512];
  uint8_t start_header_block[512];
  uint8_t end_header_block[512];
  char str[512];
  const char *filename = 0;
  uint32_t i, j;
  int e;
  /* Block parser. */
  int pos, remaining = 0;
  int32_t frame[KUM_6D6_MAX_CHANNEL_COUNT];
  uint64_t frame_counter = 0;

  int progress = 1;

  i18n_set_lang(getenv("LANG"));

  program = argv[0];
  parse_options(&argc, &argv, OPTIONS(
    FLAG('p', "progress", progress, 1),
    FLAG('q', "no-progress", progress, 0),
    FLAG_CALLBACK('h', "help", help)
  ));

  /* Set input file. */
  if (argc == 3) {
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
  uid_t uid = getuid();
  if (uid > 0 && setuid(uid) < 0) {
    fprintf(stderr, "%s", i18n->could_not_restore_uid);
    exit(1);
  }

  read_block(start_header_block, input);
  if (kum_6d6_header_read(&h_start, start_header_block) == -1) {
    read_block(start_header_block, input);
    if (kum_6d6_header_read(&h_start, start_header_block) == -1) {
      fprintf(stderr, "%s", i18n->malformed_6d6_header);
      exit(1);
    }
  }
  read_block(end_header_block, input);
  if (kum_6d6_header_read(&h_end, end_header_block) == -1) {
    fprintf(stderr, "%s", i18n->malformed_6d6_header);
    exit(1);
  }

  n_channels = h_start.channel_count;

  output = fopen(argv[2], "wbx");
  if (!output) {
    fprintf(stderr, i18n->could_not_create_file_ss, argv[2], i18n_error(errno));
    exit(1);
  }

  write_or_fail(output, start_header_block, 512);
  write_or_fail(output, end_header_block, 512);

  /* Frame format:
   * 0 frame_count
   * 1 sec usec
   * 2 x1 x2 x3 x4
   */

  i = 2;
  /* Skip to start of data.
   * Can not fseek, because stream might not be seekable. */
  while (i < h_start.address) {
    read_block(block, input);
    ++i;
  }
  /* Read data. */
  while (i < h_end.address) {
    if (try_read_block(block, input)) {
      goto done;
    }
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
          if (frame_counter > 0) {
            write_varint(output, 0);
            write_varint(output, frame_counter);
            frame_counter = 0;
          }
          /* Control Frame */
          if (frame[0] == 1 && frame[1] >= 0 && frame[2] >= 0 && frame[3] == 0) {
            write_varint(output, 1);
            write_varint(output, frame[1]);
            write_varint(output, frame[2]);
          } else {
            write_varint(output, 2);
            write_varint(output, zigzag_encode(frame[0]));
            write_varint(output, zigzag_encode(frame[1]));
            write_varint(output, zigzag_encode(frame[2]));
            write_varint(output, zigzag_encode(frame[3]));
          }
        } else {
          frame_counter += 1;
        }
      }
    }
    if (progress && i % 1024 == 0) {
      fprintf(stderr, "%3d%% %6.1fMB     \r", (int) (i * 100 / h_end.address), (double) i * 512 / 1000000l);
      fflush(stderr);
    }
  }
done:
  if (frame_counter > 0) {
    write_varint(output, 0);
    write_varint(output, frame_counter);
    frame_counter = 0;
  }
  if (progress) {
    fprintf(stderr, "%3d%% %6.1fMB     \n", 100, (double) h_end.address * 512 / 1000000l);
    fflush(stderr);
  }
  return 0;
}
