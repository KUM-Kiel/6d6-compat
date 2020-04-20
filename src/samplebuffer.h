// Samplebuffer by Lukas Joeressen.
// Licensed under CC0.
// https://creativecommons.org/publicdomain/zero/1.0/legalcode
#ifndef SAMPLEBUFFER_INCLUDE
#define SAMPLEBUFFER_INCLUDE

#include <stdint.h>

typedef struct {
  // Number of the next sample to be extracted from the buffer.
  int64_t sample_number;
  int pos, len, size;
  int32_t *data;
} Samplebuffer;

// Create a new sample buffer.
Samplebuffer *samplebuffer_new(void);

// Push a sample to the sample buffer. The buffer grows if necessary.
void samplebuffer_push(Samplebuffer *sb, int32_t sample);

// Return the next sample from the sample buffer.
// Returns 0 if the sample buffer is empty.
int32_t samplebuffer_pop(Samplebuffer *sb);

// Deletes a sample buffer.
void samplebuffer_destroy(Samplebuffer *sb);

#endif

#ifdef SAMPLEBUFFER_IMPLEMENTATION
#undef SAMPLEBUFFER_IMPLEMENTATION

#include <stdlib.h>
#include <stdio.h>

Samplebuffer *samplebuffer_new()
{
  Samplebuffer *sb;
  sb = malloc(sizeof(*sb));
  if (!sb) goto fail;
  sb->sample_number = 0;
  sb->pos = 0;
  sb->len = 0;
  sb->size = 4096;
  sb->data = malloc(sizeof(sb->data[0]) * 4096);
  if (!sb->data) goto fail;
  return sb;
fail:
  fprintf(stderr, "Out of memory!\n");
  exit(1);
  return 0;
}

void samplebuffer_push(Samplebuffer *sb, int32_t sample)
{
  int i;
  int32_t *buf;
  if (!sb) return;
  // Grow buffer if necessary.
  if (sb->len == sb->size) {
    buf = malloc(sizeof(*buf) * sb->size * 2);
    if (!buf) {
      fprintf(stderr, "Out of memory!\n");
      exit(1);
      return;
    }
    for (i = 0; i < sb->len; ++i) {
      buf[i] = sb->data[(sb->pos + i) % sb->size];
    }
    free(sb->data);
    sb->data = buf;
    sb->pos = 0;
    sb->size *= 2;
  }
  // Push the sample.
  sb->data[(sb->pos + sb->len) % sb->size] = sample;
  sb->len += 1;
}

int32_t samplebuffer_pop(Samplebuffer *sb)
{
  int32_t sample;
  if (!sb || !sb->len) return 0;
  sample = sb->data[sb->pos];
  sb->pos = (sb->pos + 1) % sb->size;
  sb->len -= 1;
  sb->sample_number += 1;
  return sample;
}

void samplebuffer_destroy(Samplebuffer *sb)
{
  if (sb) {
    free(sb->data);
    free(sb);
  }
}

#endif
