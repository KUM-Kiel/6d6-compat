#pragma once

#include <stdint.h>
#include <stdio.h>
#include "tai.h"

#define KUM_6D6_MAX_CHANNEL_COUNT 8

#define KUM_6D6_NONE 0
#define KUM_6D6_SYNC 1
#define KUM_6D6_SKEW 2

typedef struct {
  int version;
  uint8_t start_time[6];
  int sync_type;
  uint8_t sync_time[6];
  int64_t skew;
  uint32_t address;
  uint16_t sample_rate;
  uint64_t written_samples;
  uint64_t lost_samples;
  int channel_count;
  uint8_t gain[KUM_6D6_MAX_CHANNEL_COUNT];
  int bit_depth;
  uint8_t recorder_id[32];
  uint8_t rtc_id[32];
  uint8_t latitude[32];
  uint8_t longitude[32];
  uint8_t channel_names[KUM_6D6_MAX_CHANNEL_COUNT][32];
  uint8_t comment[512];
} kum_6d6_header;

/* Read a KUM_6D6 header from a 512 byte block.
 * Returns 0 on success and -1 on failure. */
extern int kum_6d6_header_read(kum_6d6_header *header, const void *x);

/* Write a KUM_6D6 header to a 512 byte block.
 * Returns 0 on success and -1 on failure. */
extern int kum_6d6_header_write(const kum_6d6_header *header, void *x);

typedef struct {
  void *userdata;
  kum_6d6_header start_header;
  kum_6d6_header end_header;
  FILE *file;
} kum_6d6_file;

/* Open a KUM_6D6 file.
 * Fills in the headers. */
extern int kum_6d6_file_open(kum_6d6_file *file, const char *filename);

/* Close an open KUM_6D6 file. */
extern int kum_6d6_file_close(kum_6d6_file *file);

/* Called, when a timestamp is encountered.
 * Return -1 to stop processing. */
typedef int (*kum_6d6_timestamp_callback)(kum_6d6_file *file, uint64_t sample_number, uint64_t offset);

/* Called, when sample data is encountered.
 * Return -1 to stop processing. */
typedef int (*kum_6d6_data_callback)(kum_6d6_file *file, uint64_t start, uint64_t frames);

/* Process a file. Calls the supplied callbacks.
 * Returns -1 on error or if processing was stopped. */
extern int kum_6d6_file_process(kum_6d6_file *file, kum_6d6_timestamp_callback timestamp_callback, kum_6d6_data_callback data_callback);

/* Print info about a 6D6 file to the given stream.
 * Returns -1 on error. */
extern int kum_6d6_show_info(FILE *f, kum_6d6_header *start_header, kum_6d6_header *end_header);

/* Print info about a 6D6 file to the given stream in JSON format.
 * Returns -1 on error. */
extern int kum_6d6_show_info_json(FILE *f, kum_6d6_header *start_header, kum_6d6_header *end_header);

/* Find the position of a time frame before the given time t. */
extern int64_t kum_6d6_find(FILE *f, int64_t offset, Time t, kum_6d6_header *start_header, kum_6d6_header *end_header);
