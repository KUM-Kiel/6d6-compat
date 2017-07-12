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
  kum_6d6_show_info(stdout, start_header, end_header);

  return 0;
}
