#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "6d6.h"
#include "options.h"
#include "version.h"
#include "i18n.h"
#include "i18n_error.h"

const char *program = "6d6copy";
static void help(const char *arg)
{
  fprintf(stdout, i18n->version_ss,
    KUM_6D6_COMPAT_VERSION, KUM_6D6_COMPAT_DATE);
  fprintf(stdout, i18n->usage_6d6copy_s,
    program);
  exit(1);
}

static void io_error(int x)
{
  fprintf(stderr, i18n->io_error_d, x);
  exit(1);
}

int main(int argc, char **argv)
{
  FILE *infile, *outfile;
  int64_t l, m, n, end;
  char buffer[1024*128];
  kum_6d6_header start_header[1], end_header[1];
  int offset = 0, e, progress = 1;

  i18n_set_lang(getenv("LANG"));

  program = argv[0];
  parse_options(&argc, &argv, OPTIONS(
    FLAG('p', "progress", progress, 1),
    FLAG('q', "no-progress", progress, 0),
    FLAG_CALLBACK('h', "help", help)
  ));

  if (argc != 3) help(0);

  infile = fopen(argv[1], "rb");
  if (!infile) {
    e = errno;
    snprintf(buffer, sizeof(buffer), "/dev/%s", argv[1]);
    infile = fopen(buffer, "rb");
    if (!infile) {
      fprintf(stderr, i18n->could_not_open_ss, argv[1], i18n_error(e));
      exit(1);
    }
  }
  /* Drop root privileges if we had any. */
  setuid(getuid());

  l = fread(buffer, 1, sizeof(buffer), infile);
  if (l < 1024 || kum_6d6_header_read(start_header, buffer) || kum_6d6_header_read(end_header, buffer + 512)) {
    /* Try to skip first 512 bytes. */
    if (l >= 3 * 512 &&
      kum_6d6_header_read(start_header, buffer + 512) == 0 &&
      kum_6d6_header_read(end_header, buffer + 1024) == 0) {
      offset = 512;
      l -= 512;
    } else {
      fprintf(stderr, "%s", i18n->malformed_6d6_header);
      exit(1);
    }
  }

  outfile = fopen(argv[2], "wb");
  if (!outfile) {
    fprintf(stderr, i18n->could_not_open_ss, argv[2], i18n_error(errno));
    exit(1);
  }

  end = (int64_t) end_header->address * 512;

  m = end < l ? end : l;

  l = fwrite(buffer + offset, 1, m, outfile);
  if (m != l) io_error(1);

  n = l;

  while (n < end) {
    m = (end - n) > sizeof(buffer) ? sizeof(buffer) : end - n;
    l = fread(buffer, 1, m, infile);
    if (m != l) io_error(2);
    l = fwrite(buffer, 1, m, outfile);
    if (m != l) io_error(3);
    n += m;
    if (progress) fprintf(stderr, "%2$3d%% %1$6.1f MB        \r", (double) n / 1000000, (int) (100 * n / end));
  }

  if (progress) fprintf(stderr, "%2$3d%% %1$6.1f MB        \n", (double) end / 1000000, 100);

  return 0;
}
