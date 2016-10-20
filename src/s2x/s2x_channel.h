#pragma once

#include "s2x.h"

#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#include <stdio.h>
#include <stdint.h>

typedef struct {
  s2x_header header;
  FILE *file;
  int samples;
  uint8_t buffer[4000];
} s2x_channel;

s2x_channel *s2x_channel_new(s2x_header *header, FILE *file);

void s2x_channel_push(s2x_channel *c, int32_t s);

void s2x_channel_flush(s2x_channel *c);

void s2x_channel_destroy(s2x_channel *c);
