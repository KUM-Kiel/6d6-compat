#define _FILE_OFFSET_BITS 64

#include "blockreader.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct Blockreader {
  pthread_t thread;
  pthread_mutex_t lock[1];
  pthread_cond_t condition[1];
  FILE *file;
  int done;
  unsigned int read_buffer, write_buffer;
  int64_t offset;
  int64_t count;
  int64_t blocksize;
  int64_t buffer_count;
  uint8_t buffer[1];
};

static void *blockreader_thread(void *x)
{
  Blockreader r = x;
  uint8_t *b;
  int e;

  pthread_mutex_lock(r->lock);
  if (fseek(r->file, r->offset, SEEK_SET))
    r->done = 1;
  while (!r->done) {
    b = r->buffer + r->write_buffer * r->blocksize;
    pthread_mutex_unlock(r->lock);

    e = !fread(b, r->blocksize, 1, r->file);

    pthread_mutex_lock(r->lock);
    if (e) {
      r->done = 1;
    } else {
      r->write_buffer = (r->write_buffer + 1) % r->buffer_count;
      r->count -= 1;
      if (r->count <= 0) r->done = 1;
    }
    pthread_cond_broadcast(r->condition);
    while (!r->done && r->count && ((r->write_buffer + 1) % r->buffer_count) == r->read_buffer) {
      pthread_cond_wait(r->condition, r->lock);
    }
  }
  pthread_mutex_unlock(r->lock);

  fclose(r->file);

  return 0;
}

extern Blockreader blockreader_new(const char *filename, int64_t start_offset, int64_t blocksize, int64_t count, int64_t buffer_count)
{
  Blockreader r;

  if (!filename || start_offset < 0 || blocksize < 1 || count < 0 || buffer_count < 2) return 0;

  r = malloc(sizeof(struct Blockreader) - 1 + blocksize * buffer_count);
  if (!r) return 0;

  r->done = 0;
  r->read_buffer = 0;
  r->write_buffer = 0;
  r->offset = start_offset;
  r->blocksize = blocksize;
  r->count = count;
  r->buffer_count = buffer_count;

  if (!(r->file = fopen(filename, "rb"))) {
    free(r);
    return 0;
  }

  pthread_mutex_init(r->lock, 0);
  pthread_cond_init(r->condition, 0);

  if (pthread_create(&r->thread, 0, blockreader_thread, r)) {
    pthread_cond_destroy(r->condition);
    pthread_mutex_destroy(r->lock);
    fclose(r->file);
    free(r);
    return 0;
  }

  return r;
}

extern const uint8_t *blockreader_get(Blockreader r)
{
  uint8_t *b = 0;

  if (!r) return 0;

  pthread_mutex_lock(r->lock);
  while (r->read_buffer == r->write_buffer) {
    if (r->done) goto end;
    pthread_cond_wait(r->condition, r->lock);
  }
  b = r->buffer + r->read_buffer * r->blocksize;
  r->read_buffer = (r->read_buffer + 1) % r->buffer_count;
  r->offset += r->blocksize;
end:
  pthread_cond_broadcast(r->condition);
  pthread_mutex_unlock(r->lock);
  return b;
}

extern int64_t blockreader_offset(Blockreader r)
{
  int64_t offset;

  if (!r) return -1;

  pthread_mutex_lock(r->lock);
  offset = r->offset;
  pthread_mutex_unlock(r->lock);
  return offset;
}

extern int64_t blockreader_length(Blockreader r)
{
  int64_t blocksize;

  if (!r) return -1;

  pthread_mutex_lock(r->lock);
  blocksize = r->blocksize;
  pthread_mutex_unlock(r->lock);
  return blocksize;
}

extern void blockreader_destroy(Blockreader r)
{
  if (!r) return;

  pthread_mutex_lock(r->lock);
  r->done = 1;
  pthread_cond_broadcast(r->condition);
  pthread_mutex_unlock(r->lock);

  pthread_join(r->thread, 0);

  pthread_mutex_destroy(r->lock);
  pthread_cond_destroy(r->condition);
  free(r);
}
