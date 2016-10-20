#include "s2x.h"

#include <string.h>
#include "number.h"

int s2x_header_init(s2x_header *h)
{
  if (!h) return -1;
  memset(h, 0, sizeof(*h));
  h->version = 0x100;
  return 0;
}

int s2x_header_write(s2x_header *h, void *x)
{
  if (!h || !x) return -1;
  memset(x, 0, 4096);
  st_u32_le(x, h->version);
  st_u32_le(x + 4, h->channel);
  memcpy(x + 8, h->chan_name, 24);
  memcpy(x + 32, h->sensor_name, 24);
  memcpy(x + 56, h->sensor_nr, 12);
  memcpy(x + 68, h->chan_comm, 40);
  st_u32_le(x + 108, h->channel_sensitivity);
  st_u32_le(x + 112, h->gain);
  st_u32_le(x + 116, h->sequ_nr);
  memcpy(x + 120, h->das_model, 12);
  st_u32_le(x + 132, h->ser_nr);
  st_i32_le(x + 136, h->skew);
  st_u32_le(x + 140, h->skew_time);
  st_u32_le(x + 144, h->sync_time);
  st_u32_le(x + 148, h->sync_type);
  st_u32_le(x + 152, h->start_time);
  st_u32_le(x + 156, h->stat_bit);
  st_u32_le(x + 160, h->used_bits);
  st_u32_le(x + 164, h->sample_period);
  st_u32_le(x + 168, h->exp_nr);
  memcpy(x + 172, h->exp_name, 24);
  memcpy(x + 196, h->exp_comm, 40);
  st_u32_le(x + 236, h->station_nr);
  memcpy(x + 240, h->station_name, 24);
  memcpy(x + 264, h->station_comm, 40);
  st_u32_le(x + 304, h->event_nr);
  st_u32_le(x + 308, h->filter_cnt);
  st_u32_le(x + 312, h->filter_delay);
  memcpy(x + 316, h->filter_name, 1024);
  st_u32_le(x + 1340, h->coeff_nr);
  st_u32_le(x + 1344, h->decimation);
  memcpy(x + 1348, h->coeff, 256);
  memcpy(x + 1604, h->dest, 256);
  st_i32_le(x + 1860, h->coordX);
  st_i32_le(x + 1864, h->coordY);
  return 0;
}

int s2x_header_read(s2x_header *h, const void *x)
{
  if (!h || !x) return -1;
  h->version = ld_u32_le(x);
  h->channel = ld_u32_le(x + 4);
  memcpy(h->chan_name, x + 8, 24);
  memcpy(h->sensor_name, x + 32, 24);
  memcpy(h->sensor_nr, x + 56, 12);
  memcpy(h->chan_comm, x + 68, 40);
  h->channel_sensitivity = ld_u32_le(x + 108);
  h->gain = ld_u32_le(x + 112);
  h->sequ_nr = ld_u32_le(x + 116);
  memcpy(h->das_model, x + 120, 12);
  h->ser_nr = ld_u32_le(x + 132);
  h->skew = ld_i32_le(x + 136);
  h->skew_time = ld_u32_le(x + 140);
  h->sync_time = ld_u32_le(x + 144);
  h->sync_type = ld_u32_le(x + 148);
  h->start_time = ld_u32_le(x + 152);
  h->stat_bit = ld_u32_le(x + 156);
  h->used_bits = ld_u32_le(x + 160);
  h->sample_period = ld_u32_le(x + 164);
  h->exp_nr = ld_u32_le(x + 168);
  memcpy(h->exp_name, x + 172, 24);
  memcpy(h->exp_comm, x + 196, 40);
  h->station_nr = ld_u32_le(x + 236);
  memcpy(h->station_name, x + 240, 24);
  memcpy(h->station_comm, x + 264, 40);
  h->event_nr = ld_u32_le(x + 304);
  h->filter_cnt = ld_u32_le(x + 308);
  h->filter_delay = ld_u32_le(x + 312);
  memcpy(h->filter_name, x + 316, 1024);
  h->coeff_nr = ld_u32_le(x + 1340);
  h->decimation = ld_u32_le(x + 1344);
  memcpy(h->coeff, x + 1348, 256);
  memcpy(h->dest, x + 1604, 256);
  h->coordX = ld_i32_le(x + 1860);
  h->coordY = ld_i32_le(x + 1864);
  return 0;
}

int s2x_package_header_write(s2x_package_header *h, void *x)
{
  if (!x || !h || h->type > 5 || h->channel < -1) return -1;
  st_u32_le(x, h->type);
  st_u32_le(x + 4, h->size);
  st_i32_le(x + 8, h->channel);
  return 0;
}

int s2x_package_header_read(s2x_package_header *h, const void *x)
{
  if (!h || !x) return -1;
  h->type = ld_u32_le(x);
  h->size = ld_u32_le(x + 4);
  h->channel = ld_i32_le(x + 8);
  return 0;
}
