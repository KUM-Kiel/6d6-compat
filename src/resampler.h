#ifndef RESAMPLER_HEADER
#define RESAMPLER_HEADER

#include "samplerate.h"
#include <stdint.h>

typedef void (*ResamplerCallback)(void *userdata, float *samples, int number_of_samples, int64_t start_time);

struct Resampler {
  SRC_STATE *src_state;
  double target_sample_rate;
  float in[8192];
  float out[8192];
  int samples_in;
  int got_time;
  int64_t input_time;
  int64_t output_time;
  double conversion_rate;
  ResamplerCallback callback;
  void *userdata;
};

struct Resampler *resampler_new(double target_sample_rate, ResamplerCallback cb, void *userdata);

void resampler_sample(struct Resampler *resampler, float sample);

void resampler_time(struct Resampler *resampler, int64_t time);

void resampler_done(struct Resampler *resampler);

#endif

#ifdef RESAMPLER_TEST
#define RESAMPLER_IMPLEMENTATION

#include <stdio.h>
static void cb(void *userdata, float *samples, int number_of_samples, int64_t start_time)
{
  int i;
  fprintf(stderr, "Got %d samples.\n", number_of_samples);
  for (i = 0; i < number_of_samples; ++i) {
    printf("%.6f,%.8e\n", start_time / 1e6, samples[i]);
    start_time += 1e6 / 100;
  }
}

int main()
{
  struct Resampler *resampler = resampler_new(100, cb, 0);
  int64_t t = 0;
  printf("Time,Value\n");
  resampler_time(resampler, t);
  int i, j;
  for (i = 0; i < 10; ++i) {
    for (j = 0; j < 10; ++j) {
      resampler_sample(resampler, 0);
      t += 1e6 / 20;
    }
    for (j = 0; j < 10; ++j) {
      resampler_sample(resampler, 0.2);
      t += 1e6 / 20;
    }
    resampler_time(resampler, t);
  }
  resampler_done(resampler);
}
#endif

#ifdef RESAMPLER_IMPLEMENTATION
#undef RESAMPLER_IMPLEMENTATION

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Resampler *resampler_new(double target_sample_rate, ResamplerCallback cb, void *userdata)
{
  struct Resampler *resampler = malloc(sizeof(*resampler));
  if (!resampler) return 0;
  resampler->callback = cb;
  resampler->userdata = userdata;
  resampler->samples_in = 0;
  resampler->target_sample_rate = target_sample_rate;
  resampler->got_time = 0;
  resampler->conversion_rate = 1;
  int e;
  resampler->src_state = src_new(0, 1, &e);
  if (!resampler->src_state) {
    free(resampler);
    return 0;
  }
  return resampler;
}

void resampler_sample(struct Resampler *resampler, float sample)
{
  if (!resampler->got_time) return;
  // Put the sample in the buffer.
  resampler->in[resampler->samples_in] = sample;
  resampler->samples_in += 1;
  while (resampler->samples_in == 8192) {
    SRC_DATA data = {
      .data_in = resampler->in,
      .data_out = resampler->out,
      .input_frames = 8192,
      .output_frames = 8192,
      .input_frames_used = 0,
      .output_frames_gen = 0,
      .end_of_input = 0,
      .src_ratio = resampler->conversion_rate,
    };
    // If the buffer is full, call into src and fill the output buffer.
    src_set_ratio(resampler->src_state, resampler->conversion_rate);
    int e = src_process(resampler->src_state, &data);
    if (e) {
      fprintf(stderr, "Resampler error: %s\n", src_strerror(e));
      exit(1);
    }
    // Then call the callback.
    if (resampler->callback) {
      resampler->callback(resampler->userdata, resampler->out, data.output_frames_gen, resampler->output_time);
    }
    // Then move the unused samples to the beginning and advance the time.
    int unused_frames = 8192 - data.input_frames_used;
    memmove(resampler->in, resampler->in + data.input_frames_used, sizeof(*resampler->in * unused_frames));
    resampler->samples_in = unused_frames;
    resampler->input_time += 1e6 * data.input_frames_used / (resampler->target_sample_rate / resampler->conversion_rate);
    resampler->output_time += 1e6 * data.output_frames_gen / resampler->target_sample_rate;
  }
}

void resampler_time(struct Resampler *resampler, int64_t time)
{
  if (!resampler->got_time) {
    resampler->input_time = time;
    resampler->output_time = time;
    resampler->got_time = 1;
  } else {
    // Calculate the proper rate.
    if (resampler->samples_in) {
      resampler->conversion_rate = resampler->target_sample_rate / (1e6 * resampler->samples_in / (time - resampler->input_time));
    } else {
      return;
    }
  }
}

void resampler_done(struct Resampler *resampler)
{
  SRC_DATA data;
start:
  // End the conversion.
  data.data_in = resampler->in;
  data.data_out = resampler->out;
  data.input_frames = resampler->samples_in;
  data.output_frames = 8192;
  data.input_frames_used = 0;
  data.output_frames_gen = 0;
  data.end_of_input = 1;
  data.src_ratio = resampler->conversion_rate;
  // Call into src.
  int e = src_process(resampler->src_state, &data);
  if (e) {
    fprintf(stderr, "Resampler error: %s\n", src_strerror(e));
    exit(1);
  }
  // Call the callback.
  if (resampler->callback) {
    resampler->callback(resampler->userdata, resampler->out, data.output_frames_gen, resampler->output_time);
  }
  int unused_frames = resampler->samples_in - data.input_frames_used;
  memmove(resampler->in, resampler->in + data.input_frames_used, sizeof(*resampler->in * unused_frames));
  resampler->samples_in = unused_frames;
  resampler->input_time += 1e6 * data.input_frames_used / (resampler->target_sample_rate / resampler->conversion_rate);
  resampler->output_time += 1e6 * data.output_frames_gen / resampler->target_sample_rate;
  // If there are still samples left or there was not enough space in the output, do another conversion.
  if (resampler->samples_in > 0 || data.output_frames_gen == 8192) goto start;
  // Call the callback only with the end time.
  if (resampler->callback) {
    resampler->callback(resampler->userdata, 0, 0, resampler->output_time);
  }
  // Destroy the SRC state.
  src_delete(resampler->src_state);
  // Free memory.
  free(resampler);
}

#endif
