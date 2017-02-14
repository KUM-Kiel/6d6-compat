#pragma once

#include <stdint.h>
#include <stdio.h>

/* Find the next timestamp in the given 6d6 file.
 * Pass frame_border = 1, if the file pointer is positioned at the beginning
 + of a frame in the file.
 + Stores the time and offset in the supplied fields and returns zero unless
 + there was an error. */
extern int find_timestamp(FILE *f, int64_t *t, int64_t *offset, int frame_border);
