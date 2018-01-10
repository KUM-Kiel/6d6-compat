#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "tai.h"

static uint16_t parse_u16(uint8_t *d)
{
  uint16_t n = 0;
  int i;
  for (i = 0; i < 2; ++i) {
    n <<= 8;
    n += d[i];
  }
  return n;
}

static int16_t parse_i16(uint8_t *d)
{
  return (int16_t) parse_u16(d);
}

static uint32_t parse_u32(uint8_t *d)
{
  uint32_t n = 0;
  int i;
  for (i = 0; i < 4; ++i) {
    n <<= 8;
    n += d[i];
  }
  return n;
}

static int32_t parse_i32(uint8_t *d)
{
  return (int32_t) parse_u32(d);
}

int main(int argc, char **argv)
{
  int i, r = 0;
  int a;
  FILE *f;
  uint8_t block[4096];
  int n = 0;
  int leap;
  double sample_rate = 1.0;
  Date d;
  for (i = 1; i < argc; ++i) {
    f = fopen(argv[i], "rb");
    if (!f) {
      fprintf(stderr, "Could not open '%s': %s.\n", argv[i], strerror(i));
      r = 1;
      continue;
    }
    printf("Record Number,Start Time,Time Correction,Leap Second,Number of Samples,Sample Rate\n");
    while (fread(block, sizeof(block), 1, f) == 1) {
      leap = !!(block[36] & 0x30);
      d.year = parse_u16(&block[20]);
      d.month = 1;
      d.day = parse_u16(&block[22]);
      d.hour = block[24];
      d.min = block[25];
      d.sec = block[26];
      d.usec = parse_u16(&block[28]) * 100;
      d = tai_date(tai_time(d), 0, 0);
      n = parse_u16(&block[30]);
      sample_rate = 1.0;
      a = parse_i16(&block[32]);
      if (a >= 0) {
        sample_rate *= a;
      } else {
        sample_rate /= -a;
      }
      a = parse_i16(&block[34]);
      if (a >= 0) {
        sample_rate *= a;
      } else {
        sample_rate /= -a;
      }
      printf(
        "%.6s,%04d-%02d-%02d %02d:%02d:%02d.%04d UTC,%.4fs,%d,%d,%g SPS\n",
        block,
        d.year, d.month, d.day, d.hour, d.min, d.sec, d.usec / 100,
        parse_i32(&block[40]) * 1e-4,
        leap, n, sample_rate);
    }
  }
  return r;
}
