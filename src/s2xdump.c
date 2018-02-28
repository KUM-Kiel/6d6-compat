#define _FILE_OFFSET_BITS 64
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s2x.h"
#include "number.h"

static void io_error(int e)
{
  fprintf(stderr, "There was an I/O error (%d). This is pretty bad!\n", e);
  exit(1);
}

int main(int argc, char **argv)
{
  s2x_header h;
  s2x_package_header ph;
  char *buffer = 0, x[12];
  FILE *in = 0;
  int i;

  if (argc < 2) {
    fprintf(stderr,
      "s2xdump 1.0.0\n"
      "KUM GmbH Kiel (c) 2018\n"
      "This software is released under the GPLv3.\n"
      "\n"
      "Usage: %s in.s2x\n"
      "\n"
      "Show all packets of a .s2x file.\n",
      argv[0]);
    return 1;
  }

  in = fopen(argv[1], "r+");
  if (!in) {
    fprintf(stderr, "Could not open '%s'. %s.\n", argv[1], strerror(errno));
    return 1;
  }

  while (1) {
    if (fread(x, 12, 1, in) != 1) break;
    s2x_package_header_read(&ph, x);

    if (ph.size) {
      buffer = malloc(ph.size);
      if (!buffer) {
        fprintf(stderr, "Out of memory!\n");
        return 1;
      }
      if (fread(buffer, ph.size, 1, in) != 1) io_error(1);
    }

    if (ph.type == S2X_HEADER) {
      if (ph.size != 4096) {
        fprintf(stderr, "Malformed input file.\n");
        return 1;
      }
      s2x_header_read(&h, buffer);
      for (i = sizeof(h.chan_name) - 1; i >= 0; --i) {
        if (h.chan_name[i] != ' ') break;
        h.chan_name[i] = 0;
      }
      printf("Header: Channel %d, %s, %d SPS\n", (int) ph.channel, h.chan_name, 1000000 / h.sample_period);
    } else if (ph.type == S2X_TIME) {
      if (ph.size != 4) {
        fprintf(stderr, "Malformed input file.\n");
        return 1;
      }
      printf("Time: %ld.\n", (long) ld_u32_le(buffer));
    } else if (ph.type == S2X_DATA) {
      printf("Data: Channel %d, %d Samples.\n", (int) ph.channel, (int) ph.size / 4);
    } else if (ph.type == S2X_TERMINATE) {
      printf("Terminate.\n");
      break;
    }

    if (buffer) {
      free(buffer);
      buffer = 0;
    }
  }

  fclose(in);

  return 0;
}
