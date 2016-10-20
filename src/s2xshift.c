#define _FILE_OFFSET_BITS 64
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s2x.h"
#include "number.h"

static void io_error(void)
{
  fprintf(stderr, "There was an I/O error. This is pretty bad!\n");
  exit(1);
}

int main(int argc, char **argv)
{
  s2x_header h;
  s2x_package_header ph;
  char *buffer = 0, x[12];
  FILE *in = 0, *out = 0;
  int inplace = 0;
  int offset;

  if (argc < 3) {
    fprintf(stderr,
      "s2xshift 1.0.0\n"
      "KUM GmbH Kiel (c) 2016\n"
      "This software is released under the GPLv3.\n"
      "\n"
      "Usage: %s offset in.s2x [out.s2x]\n"
      "\n"
      "Shift a s2x file in time by offset seconds.\n"
      "If the output file is ommited, s2xshift works in place on the input file.\n",
      argv[0]);
    return 1;
  } else if (argc < 4) {
    inplace = 1;
  }

  offset = atoi(argv[1]);
  if (inplace) {
    in = out = fopen(argv[2], "r+");
    if (!in) {
      fprintf(stderr, "Could not open '%s'. %s.\n", argv[2], strerror(errno));
      return 1;
    }
  } else {
    in = fopen(argv[2], "r");
    if (!in) {
      fprintf(stderr, "Could not open '%s'. %s.\n", argv[2], strerror(errno));
      return 1;
    }
    out = fopen(argv[3], "w");
    if (!out) {
      fprintf(stderr, "Could not open '%s'. %s.\n", argv[3], strerror(errno));
      return 1;
    }
  }

  while (1) {
    if (fread(x, 12, 1, in) != 1) io_error();
    s2x_package_header_read(&ph, x);

    if (ph.size && (!inplace || ph.type == S2X_HEADER || ph.type == S2X_TIME)) {
      buffer = malloc(ph.size);
      if (!buffer) {
        fprintf(stderr, "Out of memory!\n");
        return 1;
      }
      if (fread(buffer, ph.size, 1, in) != 1) io_error();
    } else if (ph.size) {
      if (fseek(in, ph.size, SEEK_CUR)) io_error();
    }

    if (ph.type == S2X_HEADER) {
      if (ph.size != 4096) {
        fprintf(stderr, "Malformed input file.\n");
        return 1;
      }
      s2x_header_read(&h, buffer);
      h.skew_time += offset;
      h.sync_time += offset;
      h.start_time += offset;
      s2x_header_write(&h, buffer);
    } else if (ph.type == S2X_TIME) {
      if (ph.size != 4) {
        fprintf(stderr, "Malformed input file.\n");
        return 1;
      }
      st_u32_le(buffer, ld_u32_le(buffer) + offset);
    }

    if (!inplace) {
      if (fwrite(x, 12, 1, out) != 1) io_error();
      if (ph.size) {
        if (fwrite(buffer, ph.size, 1, out) != 1) io_error();
      }
    } else if (ph.type == S2X_HEADER || ph.type == S2X_TIME) {
      if (fseek(out, -((long) ph.size), SEEK_CUR) == -1) io_error();
      if (fwrite(buffer, ph.size, 1, out) != 1) io_error();
    }

    if (buffer) {
      free(buffer);
      buffer = 0;
    }

    if (ph.type == 4) break;
  }

  if (inplace) {
    fclose(in);
  } else {
    fclose(in);
    fclose(out);
  }

  return 0;
}
