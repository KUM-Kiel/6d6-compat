#define _FILE_OFFSET_BITS 64
#include <stdint.h>
#include <stdio.h>

static int read_i32_be(FILE *f, int32_t *i)
{
  int32_t t = 0;
  int k, c;
  if (!f) return -1;
  for (k = 0; k < 4; ++k) {
    if ((c = fgetc(f)) == EOF) return -1;
    t = ((t >> 8) & 0xffffff) | (c << 24);
  }
  if (i) *i = t;
  return 0;
}

enum {
  r1, r2, r3, r4,
  data, ts,
  other1, other2, other3
};

int find_timestamp(FILE *f, int64_t *t, int64_t *offset, int frame_border)
{
  int32_t x;
  int state = frame_border ? data : r4;
  if (!f) return -1;
  while (1) {
    if (read_i32_be(f, &x)) return -1;
    switch (state) {
    case r1:
      state = (x & 1) ? r4 : data;
      break;
    case r2:
      state = (x & 1) ? r4 : r1;
      break;
    case r3:
      state = (x & 1) ? r4 : r2;
      break;
    case r4:
      state = (x & 1) ? r4 : r3;
      break;
    case data:
      state = (x & 1) ? (x == 1 ? ts : other3) : data;
      break;
    case ts:
      if (t) *t = x;
      if (offset) *offset = ftell(f) - 8; /* XXX */
      return 0;
    case other3:
      state = other2;
      break;
    case other2:
      state = other1;
      break;
    case other1:
      state = data;
      break;
    default: return -1;
    }
  }
  return -1;
}
