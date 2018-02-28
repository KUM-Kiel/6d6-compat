// Sample tracker module.
// The sample tracker tracks the time of samples, while also suppressing jitter
// in the sample clock.
// The sample tracker is fed timestamps which might include jitter and it
// estimates the real time of every sample based on previous timestamps.

#ifndef SAMPLE_TRACKER_HEADER
#define SAMPLE_TRACKER_HEADER

#include <stdint.h>

typedef struct {
  int64_t sample_number;
  int64_t time;
  int64_t error;
  int64_t error_integral;
  int64_t steer;
  int64_t sample_time;
  int64_t base_time;
} SampleTracker;

// Initalise the SampleTracker with the given sample rate.
int sample_tracker_init(SampleTracker *st, double sample_rate);

// Set the time of the next sample. If the time is inconsistent, it will be
// smoothed. This must be called before the first call of sample_tracker_sample
// with the time of the first sample.
int sample_tracker_time(SampleTracker *st, int64_t t);

// Get the time of the next sample.
int64_t sample_tracker_sample(SampleTracker *st);

#endif

#ifdef SAMPLE_TRACKER_TEST

#define SAMPLE_TRACKER_IMPLEMENTATION
#include <stdio.h>

uint32_t x32 = 314159265;
uint32_t xorshift32(int n)
{
  x32 ^= x32 << 13;
  x32 ^= x32 >> 17;
  x32 ^= x32 << 5;
  return x32 % n;
}

int main()
{
  int64_t i, j, t;
  SampleTracker st[1];

  sample_tracker_init(st, 100);

  printf("Real,Estimate\n");

  t = 0;
  for (i = 0; i < 1000; ++i) {
    // The time is retrieved with jitter.
    sample_tracker_time(st, t + xorshift32(101) - 50);
    for (j = 0; j < 100; ++j) {
      printf("%lld,%lld\n", (long long) t, (long long) sample_tracker_sample(st));
      // And the sample rate might not be exact.
      t += 10001;
    }
  }

  return 0;
}

#endif

#ifdef SAMPLE_TRACKER_IMPLEMENTATION
#undef SAMPLE_TRACKER_IMPLEMENTATION

#define SAMPLE_TRACKER_RESOLUTION 8

int sample_tracker_init(SampleTracker *st, double sample_rate)
{
  st->time = 0;
  st->base_time = 1e6 / sample_rate * ((int64_t) 1 << SAMPLE_TRACKER_RESOLUTION);
  st->sample_time = st->base_time = st->base_time;
  st->error = 0;
  st->error_integral = 0;
  st->steer = 0;
  st->sample_number = 0;
  return 0;
}

int sample_tracker_time(SampleTracker *st, int64_t t)
{
  int64_t e;
  if (st->sample_number == 0) {
    st->time = t << SAMPLE_TRACKER_RESOLUTION;
  } else {
    e = (t << SAMPLE_TRACKER_RESOLUTION) - st->time;
    st->error += (e - st->error) >> 4;
    st->error_integral += st->error;
    st->steer = (64 * st->error + st->error_integral) >> 16;
    // Limit steering.
    if (st->steer > st->base_time / 100)
      st->steer = st->base_time / 100;
    if (st->steer < -st->base_time / 100)
      st->steer = -st->base_time / 100;
    st->sample_time = st->base_time + st->steer;
  }
  return 0;
}

int64_t sample_tracker_sample(SampleTracker *st)
{
  int64_t t = st->time >> SAMPLE_TRACKER_RESOLUTION;
  st->time += st->sample_time;
  st->sample_number += 1;
  return t;
}

#endif
