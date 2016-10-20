#include "s2x_channel.h"
#include <stdlib.h>
#include <string.h>
#include "number.h"

static void fatal(const char *s)
{
  fprintf(stderr, "%s\n", s);
  exit(1);
}

s2x_channel *s2x_channel_new(s2x_header *header, FILE *file)
{
  uint8_t x[4108];
  s2x_package_header h;
  s2x_channel *c = malloc(sizeof(s2x_channel));
  if (!c) fatal("Out of memory");
  memcpy(&c->header, header, sizeof(s2x_header));
  c->file = file;
  c->samples = 0;
  h.type = S2X_HEADER;
  h.size = 4096;
  h.channel = c->header.channel;
  s2x_package_header_write(&h, x);
  s2x_header_write(header, x + 12);
  if (fwrite(x, 4108, 1, file) != 1) fatal("I/O error");
  return c;
}

void s2x_channel_push(s2x_channel *c, int32_t s)
{
  if (c->samples >= sizeof(c->buffer) / 4) s2x_channel_flush(c);
  st_i32_le(c->buffer + c->samples * 4, s);
  c->samples += 1;
}

void s2x_channel_flush(s2x_channel *c)
{
  uint8_t x[12];
  s2x_package_header h;
  if (c->samples) {
    h.type = S2X_DATA;
    h.size = c->samples * 4;
    h.channel = c->header.channel;
    s2x_package_header_write(&h, x);
    if (fwrite(x, 12, 1, c->file) != 1) fatal("I/O error");
    if (fwrite(c->buffer, c->samples * 4, 1, c->file) != 1) fatal("I/O error");
    c->samples = 0;
  }
}

void s2x_channel_destroy(s2x_channel *c)
{
  free(c);
}
