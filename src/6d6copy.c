#define _FILE_OFFSET_BITS 64
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "6d6.h"
#include "blockreader.h"
#include "options.h"

static const char *program = "6d6copy";
static void help(const char *arg)
{
  fprintf(stderr, "Usage: %s [-p|--progress] /dev/sdX > out.6d6\n", program);
  exit(1);
}

int main(int argc, char **argv)
{
  kum_6d6_header h;
  const uint8_t *block;
  const char *filename;
  char str[512];
  Blockreader sdcard = 0;
  uint32_t i;
  int progress = 0;
  program = argv[0];
  parse_options(&argc, &argv, OPTIONS(
    FLAG('p', "progress", progress, 1),
    FLAG_CALLBACK('h', "help", help)
  ));
  if (argc < 2 || isatty(1)) {
    help(0);
    return 1;
  }
  filename = argv[1];
  sdcard = blockreader_new(filename, 0, 512, 2, 2);
  if (!sdcard) {
    snprintf(str, sizeof(str), "/dev/%s", argv[1]);
    filename = str;
    sdcard = blockreader_new(filename, 0, 512, 2, 2);
  }
  if (!sdcard) {
    fprintf(stderr, "Could not open '%s'. %s.\n", argv[1], strerror(errno));
    return 1;
  }
  block = blockreader_get(sdcard);
  if (!block) return 1;
  if (kum_6d6_header_read(&h, block) < 0) return 1;
  block = blockreader_get(sdcard);
  if (!block) return 1;
  if (kum_6d6_header_read(&h, block) < 0) return 1;
  blockreader_destroy(sdcard);
  sdcard = blockreader_new(filename, 0, 512, h.address, 8192);
  if (!sdcard) {
    fprintf(stderr, "Could not reopen '%s'. %s.\n", argv[1], strerror(errno));
    return 1;
  }
  for (i = 0; i < h.address; ++i) {
    block = blockreader_get(sdcard);
    if (!block) return 1;
    if (fwrite(block, 512, 1, stdout) == 0) return 1;
    if (progress && i % 1024 == 0) {
      fprintf(stderr, "%3d%% %6.1fMB     \r", (int) (i * 100 / h.address), (double) i * 512 / 1000000);
      fflush(stderr);
    }
  }
  if (progress) {
    fprintf(stderr, "%3d%% %6.1fMB     \n", 100, (double) h.address * 512 / 1000000);
  }
  blockreader_destroy(sdcard);
  return 0;
}
