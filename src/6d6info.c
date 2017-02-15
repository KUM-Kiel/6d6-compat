#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <inttypes.h>
#include "6d6.h"
#include "bcd.h"
#include "options.h"
#include "version.h"

const char *program = "6d6info";
static void help(const char *arg)
{
  fprintf(stderr, "Version %s (%s)\n",
    KUM_6D6_COMPAT_VERSION, KUM_6D6_COMPAT_DATE);
  fprintf(stderr,
    "Usage: %s /dev/sdX1\n"
    "\n"
    "The program '6d6info' shows some information about a 6D6 recording.\n"
    "You can either use it on a StiK or SD card or to get some information\n"
    "about a .6d6 file on your hard drive.\n"
    "\n"
    "The parameter is the file or device you want to inspect.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "\n"
    "Inspect the StiK in '/dev/sdb1':\n"
    "\n"
    "  $ 6d6info /dev/sdb1\n"
    "\n"
    "Inspect the file 'station-007.6d6' in the directory 'line-001':\n"
    "\n"
    "  $ 6d6info line-001/station-007.6d6\n",
    program);
  exit(1);
}

static int format_duration(long d, char *out, int maxlen)
{
  int l, n = 0;
  long t;
  t = d / 86400;
  if (t) {
    l = snprintf(out, maxlen, "%ldd", t);
    if (l < 0) return l;
    n += l;
  }
  d %= 86400;
  t = d / 3600;
  if (t) {
    l = snprintf(out + n, maxlen - n, "%s%ldh", n ? " " : "", t);
    if (l < 0) return l;
    n += l;
  }
  d %= 3600;
  t = d / 60;
  if (t) {
    l = snprintf(out + n, maxlen - n, "%s%ldm", n ? " " : "", t);
    if (l < 0) return l;
    n += l;
  }
  d %= 60;
  if (d) {
    l = snprintf(out + n, maxlen - n, "%s%lds", n ? " " : "", d);
    if (l < 0) return l;
    n += l;
  }
  return n;
}

static int indent(const char *padding, const char *text, char *out, int maxlen)
{
  int n = 0;
  const char *p;
  int need_padding = 0;
  while (*text && n < maxlen - 1) {
    if (need_padding && *text != '\n') {
      p = padding;
      while (*p) {
        out[n++] = *(p++);
        if (n >= maxlen - 1) goto done;
      }
      need_padding = 0;
    }
    out[n++] = *text;
    if (*text == '\n') {
      need_padding = 1;
    }
    ++text;
  }
done:
  out[n] = 0;
  return n;
}

int main(int argc, char **argv)
{
  FILE *infile;
  int64_t l;
  char buffer[512*3];
  kum_6d6_header start_header[1], end_header[1];
  int e;

  program = argv[0];
  parse_options(&argc, &argv, OPTIONS(
    FLAG_CALLBACK('h', "help", help)
  ));

  if (argc != 2) help(0);

  infile = fopen(argv[1], "rb");
  if (!infile) {
    e = errno;
    snprintf(buffer, sizeof(buffer), "/dev/%s", argv[1]);
    infile = fopen(buffer, "rb");
    if (!infile) {
      fprintf(stderr, "Could not open '%s': %s.\n", argv[1], strerror(e));
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
    } else {
      fprintf(stderr, "Invalid file '%s'.\n", argv[1]);
      exit(1);
    }
  }

  /* Show all the info. */
  printf("    6D6 S/N: %s\n", start_header->recorder_id);
  bcd_format((char *) start_header->start_time, buffer, sizeof(buffer));
  printf(" Start Time: %s\n", buffer);
  bcd_format((char *) end_header->start_time, buffer, sizeof(buffer));
  printf("   End Time: %s\n", buffer);
  bcd_format((char *) start_header->sync_time, buffer, sizeof(buffer));
  printf("  Sync Time: %s\n", buffer);
  if (end_header->sync_type == KUM_6D6_SKEW) {
    bcd_format((char *) end_header->sync_time, buffer, sizeof(buffer));
    printf("  Skew Time: %s\n", buffer);
    printf("       Skew: %" PRId64 "µs\n", end_header->skew);
  }
  format_duration(bcd_diff((char *) start_header->start_time, (char *) end_header->start_time), buffer, sizeof(buffer));
  printf("   Duration: %s\n", buffer);
  printf("Sample Rate: %d SPS\n", start_header->sample_rate);
  printf("       Size: %.1f MB\n", end_header->address * 512.0 / 1e6);

  indent("             ", (char *) start_header->comment, buffer, sizeof(buffer));
  printf("    Comment: %s\n", buffer);

  return 0;
}
