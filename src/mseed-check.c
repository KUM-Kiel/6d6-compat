#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "tai.h"

int main(int argc, char **argv)
{
  int i, r = 0;
  int a;
  FILE *f;
  uint8_t block[4096];
  int n = 0;
  double sample_rate = 1.0, x;
  Date d;
  Time t, tt = 0;
  for (i = 1; i < argc; ++i) {
    f = fopen(argv[i], "rb");
    if (!f) {
      fprintf(stderr, "Could not open '%s': %s.\n", argv[i], strerror(i));
      r = 1;
      continue;
    }
    while (fread(block, sizeof(block), 1, f) == 1) {
      if (block[36] & 0x30) {
        printf("Record %.6s contains a leap second.\n", block);
      }
      d.year = ((int) block[20] << 8) + block[21];
      d.month = 1;
      d.day = ((int) block[22] << 8) + block[23];
      d.hour = block[24];
      d.min = block[25];
      d.sec = block[26];
      d.usec = (((int) block[28] << 8) + block[29]) * 100;
      t = tai_time(d);
      if (n > 0) {
        x = n - sample_rate * (t - tt) * 1e-6;
        printf("%d,%d,%+.2f\n", (int) (t - tt), n, x);
      }
      tt = t;
      n = ((int) block[30] << 8) + block[31];
      sample_rate = 1.0;
      a = ((int) block[32] << 8) + block[33];
      if (a >= 0) {
        sample_rate *= a;
      } else {
        sample_rate /= -a;
      }
      a = ((int) block[34] << 8) + block[35];
      if (a >= 0) {
        sample_rate *= a;
      } else {
        sample_rate /= -a;
      }
    }
  }
  return r;
}
