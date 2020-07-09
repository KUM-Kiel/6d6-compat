// MiniSeed by Lukas Joeressen.
// Licensed under CC0-1.0.
// https://creativecommons.org/publicdomain/zero/1.0/legalcode
#ifndef MINISEED_INCLUDE
#define MINISEED_INCLUDE

#include <stdint.h>

#define MINISEED_RECORD_SIZE 4096

typedef struct {
  uint8_t data[MINISEED_RECORD_SIZE];
} MiniSeedRecord;

// Initialize a MiniSeed record with a record number.
int miniseed_record_init(MiniSeedRecord *r, int record_number);

// Set the info fields on a MiniSeed record
int miniseed_record_set_info(MiniSeedRecord *r, const char *station, const char *location, const char *channel, const char *network);

// Set the time of the first sample in the record.
// `frac` and `correction` each have a resolution of 0.0001 seconds.
int miniseed_record_set_start_time(MiniSeedRecord *r, int year, int month, int day, int hour, int min, int sec, int frac, int correction);

// Set the sample rate of the record.
int miniseed_record_set_sample_rate(MiniSeedRecord *r, double sample_rate);

// Push a sample to the record. Returns -1 if the record is full.
int miniseed_record_push_sample(MiniSeedRecord *r, int32_t sample);

// Mark a record as containing a leap second.
int miniseed_record_set_leapsec(MiniSeedRecord *r, int leapsec);

#endif

#ifdef MINISEED_IMPLEMENTATION
#undef MINISEED_IMPLEMENTATION

#include <string.h>

int miniseed_record_init(MiniSeedRecord *r, int record_number)
{
  int i;
  if (!r) return -1;
  memset(r->data, 0, MINISEED_RECORD_SIZE);
  for (i = 0; i < 6; ++i) {
    r->data[5 - i] = '0' + (record_number % 10);
    record_number /= 10;
  }
  r->data[6] = 'D';
  memset(r->data + 7, ' ', 13);
  // Number of blockettes.
  r->data[39] = 1;
  // Start of data.
  r->data[45] = 0x40;
  // Start of first blockette.
  r->data[47] = 0x30;
  // Blockette 1000.
  r->data[48] = 0x03;
  r->data[49] = 0xe8;
  // Encoding format.
  r->data[52] = 3;
  // Word order.
  r->data[53] = 1;
  // Data record length.
  r->data[54] = 12;
  return 0;
}

int miniseed_record_set_info(MiniSeedRecord *r, const char *station, const char *location, const char *channel, const char *network)
{
  int i;
  if (!r) return -1;
  if (station) {
    for (i = 0; i < 5; ++i) {
      r->data[8 + i] = (*station ? *(station++) : ' ');
    }
  }
  if (location) {
    for (i = 0; i < 2; ++i) {
      r->data[13 + i] = (*location ? *(location++) : ' ');
    }
  }
  if (channel) {
    for (i = 0; i < 3; ++i) {
      r->data[15 + i] = (*channel ? *(channel++) : ' ');
    }
  }
  if (network) {
    for (i = 0; i < 2; ++i) {
      r->data[18 + i] = (*network ? *(network++) : ' ');
    }
  }
  return 0;
}

#define miniseed__leapyear(year) ((year) % 400 == 0 || ((year) % 4 == 0 && (year) % 100 != 0))
int miniseed_record_set_start_time(MiniSeedRecord *r, int year, int month, int day, int hour, int min, int sec, int frac, int correction)
{
  int yday;
  if (!r) return -1;
  if (month < 2) {
    yday = day;
  } else if (month == 2) {
    yday = day + 31;
  } else {
    yday = day + 59 + miniseed__leapyear(year) + ((month - 3) * 153 + 2) / 5;
  }
  r->data[20] = year >> 8;
  r->data[21] = year;
  r->data[22] = yday >> 8;
  r->data[23] = yday;
  r->data[24] = hour;
  r->data[25] = min;
  r->data[26] = sec;
  r->data[27] = 0;
  r->data[28] = frac >> 8;
  r->data[29] = frac;
  r->data[40] = correction >> 24;
  r->data[41] = correction >> 16;
  r->data[42] = correction >> 8;
  r->data[43] = correction;
  return 0;
}

int miniseed_record_set_sample_rate(MiniSeedRecord *r, double sample_rate)
{
  int a, b;
  if (!r) return -1;
  // TODO: Find the best possible approximation.
  if (sample_rate > 32767 || sample_rate < 1) return -1;
  a = sample_rate;
  b = 1;
  r->data[32] = (a >> 8);
  r->data[33] = a;
  r->data[34] = (b >> 8);
  r->data[35] = b;
  return 0;
}

int miniseed_record_push_sample(MiniSeedRecord *r, int32_t sample)
{
  int num_samples, i;
  if (!r) return -1;
  num_samples = r->data[30] * 256 + r->data[31];
  if (num_samples >= 1000) return -1;
  i = 64 + num_samples * 4;
  r->data[i] = sample >> 24;
  r->data[i + 1] = sample >> 16;
  r->data[i + 2] = sample >> 8;
  r->data[i + 3] = sample;
  num_samples += 1;
  r->data[30] = num_samples >> 8;
  r->data[31] = num_samples;
  return 0;
}

int miniseed_record_set_leapsec(MiniSeedRecord *r, int leapsec)
{
  if (!r) return -1;
  r->data[36] &= ~0x30;
  if (leapsec) {
    r->data[36] |= leapsec > 0 ? 0x10 : 0x20;
  }
  return 0;
}

#endif
