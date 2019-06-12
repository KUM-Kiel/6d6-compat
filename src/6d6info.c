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
#include "tai.h"
#include "i18n.h"
#include "i18n_error.h"

const char *program = "6d6info";
static void help(const char *arg)
{
  fprintf(stdout, i18n->version_ss,
    KUM_6D6_COMPAT_VERSION, KUM_6D6_COMPAT_DATE);
  fprintf(stdout, i18n->usage_6d6info_s,
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
  int json = 0;

  i18n_set_lang(getenv("LANG"));

  int outdated = tai_leapsecs_need_update(tai_now());
  if (outdated) {
    fprintf(stderr, "%s", i18n->leapsec_outdated);
  }

  program = argv[0];
  parse_options(&argc, &argv, OPTIONS(
    FLAG_CALLBACK('h', "help", help),
    FLAG(0, "json", json, 1)
  ));

  if (argc != 2) help(0);

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
  uid_t uid = getuid();
  if (uid > 0 && setuid(uid) < 0) {
    fprintf(stderr, "%s", i18n->could_not_restore_uid);
    exit(1);
  }

  l = fread(buffer, 1, sizeof(buffer), infile);
  if (l < 1024 || kum_6d6_header_read(start_header, buffer) || kum_6d6_header_read(end_header, buffer + 512)) {
    /* Try to skip first 512 bytes. */
    if (l >= 3 * 512 &&
      kum_6d6_header_read(start_header, buffer + 512) == 0 &&
      kum_6d6_header_read(end_header, buffer + 1024) == 0) {
    } else {
      fprintf(stderr, "%s", i18n->malformed_6d6_header);
      exit(1);
    }
  }

  /* Show all the info. */
  if (json) {
    kum_6d6_show_info_json(stdout, start_header, end_header);
  } else {
    kum_6d6_show_info(stdout, start_header, end_header);
  }

  return 0;
}
