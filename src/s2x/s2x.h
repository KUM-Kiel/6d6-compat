#pragma once

#include <stdint.h>

typedef struct {
  uint32_t version;
  uint32_t channel;
  uint8_t chan_name[24];
  uint8_t sensor_name[24];
  uint8_t sensor_nr[12];
  uint8_t chan_comm[40];
  uint32_t channel_sensitivity;
  uint32_t gain;
  uint32_t sequ_nr;
  uint8_t das_model[12];
  uint32_t ser_nr;
  int32_t skew;
  uint32_t skew_time;
  uint32_t sync_time;
  uint32_t sync_type;
  uint32_t start_time;
  uint32_t stat_bit;
  uint32_t used_bits;
  uint32_t sample_period; /* In microseconds. Do not trust the manual! */
  uint32_t exp_nr;
  uint8_t exp_name[24];
  uint8_t exp_comm[40];
  uint32_t station_nr;
  uint8_t station_name[24];
  uint8_t station_comm[40];
  uint32_t event_nr;
  uint32_t filter_cnt;
  uint32_t filter_delay;
  uint8_t filter_name[16][64];
  uint32_t coeff_nr;
  uint32_t decimation;
  uint8_t coeff[256];
  uint8_t dest[256];
  int32_t coordX;
  int32_t coordY;
} s2x_header;

/* Populate an s2x_header with default values. */
extern int s2x_header_init(s2x_header *h);

/* Write the s2x_header to the 4096 byte memory area x. */
extern int s2x_header_write(s2x_header *h, void *x);

/* Read the s2x_header from the 4096 byte memory area x. */
extern int s2x_header_read(s2x_header *h, const void *x);

enum {
  S2X_DATA = 0,
  S2X_META = 1,
  S2X_HEADER = 2,
  S2X_TIME = 3,
  S2X_TERMINATE = 4,
  S2X_SHOT = 5
};

typedef struct {
  uint32_t type;
  uint32_t size;
  int32_t channel;
} s2x_package_header;

/* Write a s2x_package_header to the 12 byte memory area x. */
extern int s2x_package_header_write(s2x_package_header *h, void *x);

/* Read a s2x_package_header from the 12 byte memory area x. */
extern int s2x_package_header_read(s2x_package_header *h, const void *x);
