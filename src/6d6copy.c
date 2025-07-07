#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include "6d6.h"
#include "options.h"
#include "version.h"
#include "i18n.h"
#include "i18n_error.h"
#include "monotonic-time.h"
#include "progress.h"

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

static int append_string(char *s, const char *s2, size_t n)
{
  size_t l1 = strlen(s);
  size_t l2 = strlen(s2);
  if (l1 + l2 < n) {
    memcpy(s + l1, s2, l2);
    memset(s + l1 + l2, 0, n - l1 - l2);
    return 0;
  } else {
    return -1;
  }
}

static void unescape(char *s)
{
  int r = 0, w = 0;
  while (s[r]) {
    if (s[r] == '\\') {
      r += 1;
      if (s[r] == 'n') {
        s[w] = '\n';
      } else {
        s[w] = s[r];
      }
    } else {
      s[w] = s[r];
    }
    r += 1;
    w += 1;
  }
  s[w] = 0;
}

int main(int argc, char **argv)
{
  FILE *infile, *outfile;
  int64_t l, m, n, end;
  char buffer[1024*128];
  kum_6d6_header start_header[1], end_header[1];
  int offset = 0, e, progress = 1;
  char *append_comment = 0;
  int64_t t0, t1, t, _50ms;

  i18n_set_lang(getenv("LANG"));

  program = argv[0];
  parse_options(&argc, &argv, OPTIONS(
    FLAG('p', "progress", progress, 1),
    FLAG('q', "no-progress", progress, 0),
    FLAG(0, "json-progress", progress, 2),
    PARAMETER(0, "append-comment", append_comment),
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

  // Update the headers.
  if (append_comment) {
    unescape(append_comment);
    if (append_string((char *) start_header->comment, append_comment, sizeof(start_header->comment)) < 0 || kum_6d6_header_write(start_header, buffer + offset) < 0) {
      fprintf(stderr, "%s", i18n->comment_too_long);
      exit(1);
    }
  }

  m = end < l ? end : l;

  l = fwrite(buffer + offset, 1, m, outfile);
  if (m != l) io_error(1);

  n = l;

  _50ms = monotonic_time_ms(50);
  t0 = t1 = monotonic_time();
  if (progress == 2) {
    fprintf(stdout, "{\"done\":0,\"total\":%"PRId64",\"elapsed\":0}\n", end);
    fflush(stdout);
  }
  while (n < end) {
    m = (end - n) > sizeof(buffer) ? sizeof(buffer) : end - n;
    l = fread(buffer, 1, m, infile);
    if (m != l) io_error(2);
    l = fwrite(buffer, 1, m, outfile);
    if (m != l) io_error(3);
    n += m;
    if (progress == 1) {
      progress_update(n, end);
    } else if (progress == 2) {
      t = monotonic_time();
      if (t - t1 >= _50ms) {
        fprintf(stdout,
          "{\"done\":%"PRId64",\"total\":%"PRId64",\"elapsed\":%"PRId64"}\n",
          n, end, (t - t0) * 50 / _50ms);
        fflush(stdout);
        t1 = t;
      }
    }
  }

  if (progress == 1) {
    progress_complete(end);
  } else if (progress == 2) {
    t = monotonic_time();
    fprintf(stdout,
          "{\"done\":%"PRId64",\"total\":%"PRId64",\"elapsed\":%"PRId64"}\n",
          end, end, (t - t0) * 50 / _50ms);
    fflush(stdout);
  }

  return 0;
}
