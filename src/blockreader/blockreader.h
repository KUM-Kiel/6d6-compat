#pragma once

#include <stdint.h>

typedef struct Blockreader *Blockreader;

/* Create a new Blockreader. */
extern Blockreader blockreader_new(const char *filename, int64_t start_offset, int64_t blocksize, int64_t count, int64_t buffer_count);

/* Get the next block.
 * The pointer is valid until blockreader_get is called again
 * or the Blockreader is destroyed by blockreader_destroy.
 * In case of an error NULL is returned. */
extern const uint8_t *blockreader_get(Blockreader r);

/* Get the offset of the last block returned by blockreader_get. */
extern int64_t blockreader_offset(Blockreader r);

/* Get the length of the last block returned by blockreader_get. */
extern int64_t blockreader_length(Blockreader r);

/* Destroy the Blockreader and free all of its resources. */
extern void blockreader_destroy(Blockreader r);
