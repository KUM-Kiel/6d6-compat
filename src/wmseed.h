#ifndef WMSEED_INCLUDE
#define WMSEED_INCLUDE

#include <stdint.h>
#include <stdio.h>
#include "miniseed.h"
#include "tai.h"
#include "samplebuffer.h"
#include "resampler.h"

typedef struct {
  int64_t cut;
  int64_t sample_number;
  char *template;
  char *station, *location, *channel, *network;
  double sample_rate;
  Samplebuffer *sb;
  int record_number;
  Time record_time;
  MiniSeedRecord record[1];
  int data_pending;
  FILE *output;
  FILE *logfile;
  Time last_t;
  Time start_time;
  Time end_time;
  int first_file_created;
  int64_t last_sn;
  struct Resampler *resampler;
} WMSeed;

// Create a new MiniSEED writer.
WMSeed *wmseed_new(FILE *logfile, const char *template, const char *station, const char *location, const char *channel, const char *network, double sample_rate, int64_t cut, int resampling);
// Close the MiniSEED writer when done.
int wmseed_destroy(WMSeed *w);
// Push a sample to the MiniSEED writer.
int wmseed_sample(WMSeed *w, int32_t sample);
// Set the time of the next sample.
int wmseed_time(WMSeed *w, Time t);
// Limit the start time.
int wmseed_start_time(WMSeed *w, Time t);
// Limit the end time.
int wmseed_end_time(WMSeed *w, Time t);

#endif

#ifdef WMSEED_IMPLEMENTATION
#undef WMSEED_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include "i18n.h"

#if __GNUC__ || __clang__
__attribute__((format(printf, 3, 4)))
#endif
static void wmseed__log(WMSeed *w, FILE *f, const char *format, ...)
{
  va_list args;
  if (w && w->logfile) {
    va_start(args, format);
    vfprintf(w->logfile, format, args);
    va_end(args);
    fflush(w->logfile);
  }
  va_start(args, format);
  vfprintf(f, format, args);
  va_end(args);
  fflush(f);
}

static void *wmseed__allocate(WMSeed *w, void *x, size_t size)
{
  void *t;
  if (size) {
    t = realloc(x, size);
    if (!t) {
      wmseed__log(w, stderr, "%s", i18n->out_of_memory);
      exit(1);
    }
    return t;
  } else {
    free(x);
    return 0;
  }
}

static char *wmseed__strdup(WMSeed *w, const char *s)
{
  char *s2;
  if (s) {
    s2 = wmseed__allocate(w, 0, strlen(s) + 1);
    strcpy(s2, s);
    return s2;
  } else {
    return 0;
  }
}

#if __GNUC__ || __clang__
__attribute__((format(printf, 3, 4)))
#endif
static char *wmseed__strappend(WMSeed *w, char *s, const char *format, ...)
{
  size_t l1 = s ? strlen(s) : 0, l2;
  va_list args;
  va_start(args, format);
  l2 = vsnprintf(0, 0, format, args);
  va_end(args);
  s = wmseed__allocate(w, s, l1 + l2 + 1);
  va_start(args, format);
  vsnprintf(s + l1, l2 + 1, format, args);
  va_end(args);
  return s;
}

static char *wmseed__filename(WMSeed *w, Time t)
{
  int yday;
  Date d = tai_date(t, &yday, 0);
  char *s = 0;
  char *tmpl = w->template;
  while (*tmpl) {
    if (*tmpl == '%') {
      ++tmpl;
      switch (*tmpl) {
      case '%':
        s = wmseed__strappend(w, s, "%%");
        break;
      case 'y':
        s = wmseed__strappend(w, s, "%04d", d.year);
        break;
      case 'm':
        s = wmseed__strappend(w, s, "%02d", d.month);
        break;
      case 'd':
        s = wmseed__strappend(w, s, "%02d", d.day);
        break;
      case 'h':
        s = wmseed__strappend(w, s, "%02d", d.hour);
        break;
      case 'i':
        s = wmseed__strappend(w, s, "%02d", d.min);
        break;
      case 's':
        s = wmseed__strappend(w, s, "%02d", d.sec);
        break;
      case 'j':
        s = wmseed__strappend(w, s, "%03d", yday);
        break;
      case 'S':
        s = wmseed__strappend(w, s, "%s", w->station);
        break;
      case 'L':
        s = wmseed__strappend(w, s, "%s", w->location);
        break;
      case 'C':
        s = wmseed__strappend(w, s, "%s", w->channel);
        break;
      case 'N':
        s = wmseed__strappend(w, s, "%s", w->network);
        break;
      default:
        free(s);
        return 0;
      }
    } else {
      s = wmseed__strappend(w, s, "%c", *tmpl);
    }
    ++tmpl;
  }
  return s;
}

static char *wmseed__dirname(WMSeed *w, const char *path)
{
  ssize_t l = 0, x = -1;
  char *s;
  while (path[l]) {
    if (path[l] == '/') x = l;
    ++l;
  }
  if (x > 0) {
    s = wmseed__allocate(w, 0, x + 1);
    memcpy(s, path, x);
    s[x] = 0;
  } else {
    s = wmseed__allocate(w, 0, 2);
    s[0] = x < 0 ? '.' : '/';
    s[1] = 0;
  }
  return s;
}

static int wmseed__mkdir_p(WMSeed *w, const char *path)
{
  struct stat sb;
  char *p = wmseed__strdup(w, path);
  size_t i, len = strlen(p);

  // Remove trailing slash.
  if (p[len - 1] == '/') {
    p[len - 1] = 0;
  }

  for (i = 1; i < len; ++i) {
    if (p[i] == '/') {
      p[i] = 0;
      if (stat(p, &sb)) {
        if (mkdir(p, 0755)) {
          free(p);
          return -1;
        }
      } else if (!S_ISDIR(sb.st_mode)) {
        free(p);
        return -1;
      }
      p[i] = '/';
    }
  }
  if (stat(p, &sb)) {
    if (mkdir(p, 0755)) {
      free(p);
      return -1;
    }
  } else if (!S_ISDIR(sb.st_mode)) {
    free(p);
    return -1;
  }
  free(p);
  return 0;
}

int wmseed_start_time(WMSeed *w, Time t)
{
  if (!w) return -1;
  w->start_time = t;
  return 0;
}

int wmseed_end_time(WMSeed *w, Time t)
{
  if (!w) return -1;
  w->end_time = t;
  return 0;
}

static int wmseed__sample(WMSeed *w, int32_t sample)
{
  if (!w || w->last_sn < 0) return -1;
  samplebuffer_push(w->sb, sample);
  w->sample_number += 1;
  return 0;
}

static void wmseed__flush(WMSeed *w)
{
  if (w->data_pending) {
    if (!w->output) {
      wmseed__log(w, stderr, i18n->io_error_d, 20);
      exit(1);
    }
    if (fwrite(w->record->data, sizeof(w->record->data), 1, w->output) != 1) {
      wmseed__log(w, stderr, i18n->io_error_d, 21);
      exit(1);
    }
    w->data_pending = 0;
  }
}

int wmseed_destroy(WMSeed *w)
{
  if (!w) return -1;
  if (w->resampler) {
    resampler_done(w->resampler);
  }
  // Write remaining data.
  // TODO: Flush the data from the sample buffer.
  wmseed__flush(w);
  // Close files.
  if (w->output) fclose(w->output);
  // Free everything.
  free(w->template);
  free(w->station);
  free(w->location);
  free(w->channel);
  free(w->network);
  samplebuffer_destroy(w->sb);
  free(w);
  return 0;
}

static void wmseed__new_record(WMSeed *w, Time t)
{
  Date d;
  if (w->data_pending) {
    if (tai_utc_diff(w->record_time) != tai_utc_diff(t)) {
      miniseed_record_set_leapsec(w->record, 1);
    }
    wmseed__flush(w);
  }
  w->record_number += 1;
  w->record_time = t;
  w->data_pending = 0;
  miniseed_record_init(w->record, w->record_number);
  miniseed_record_set_info(w->record, w->station, w->location, w->channel, w->network);
  miniseed_record_set_sample_rate(w->record, w->sample_rate);
  d = tai_date(t, 0, 0);
  miniseed_record_set_start_time(w->record, d.year, d.month, d.day, d.hour, d.min, d.sec, d.usec / 100, 0);
}

static void wmseed__create_file(WMSeed *w, Time t)
{
  char *filename, *dirname;
  w->record_number = 0;
  wmseed__new_record(w, t);
  if (w->output) {
    fclose(w->output);
  }
  filename = wmseed__filename(w, t);
  dirname = wmseed__dirname(w, filename);
  wmseed__mkdir_p(w, dirname);
  free(dirname);
  w->output = fopen(filename, "wb");
  if (!w->output) {
    wmseed__log(w, stderr, i18n->could_not_create_file_ss, filename, strerror(errno));
    exit(1);
  } else {
    wmseed__log(w, stderr, i18n->created_file_s, filename);
  }
  free(filename);
}

// b must be positive.
#define wmseed__div(a, b) ((a) / (b) - ((a) % (b) < 0))

static int wmseed__time(WMSeed *w, Time t)
{
  int64_t off;
  int32_t sample;
  double a;
  Time tt, split_time = INT64_MAX;
  if (!w) return -1;
  if (w->last_sn == -1) {
    if (w->sample_number != 0) return -1;
    w->last_t = t;
    w->last_sn = w->sample_number;
    return 0;
  } else if (w->sample_number <= w->last_sn || t <= w->last_t) {
    return -1;
  }

  /*if (w->sample_number - w->last_sn < 1008 * 20) {
    // Don't use too many timestamps.
    return 0;
  }*/

  // Use linear interpolation.
  a = (double) (t - w->last_t) / (w->sample_number - w->last_sn);

  // Check for a cut between the two timestamps.
  // Calculate UTC offset to cut at round UTC dates and times.
  off = 1000000 * tai_utc_diff(t);
  // Since the sample `w->sample_number` is excluded, the split must not fall
  // on it. By subtracting 1 from each time, the end time `t` is excluded from
  // the range.
  // TODO: This probably needs a better fix. Maybe save the split time in the
  // WMSeed struct.
  if (w->cut && wmseed__div(w->last_t - 1 - off, w->cut) != wmseed__div(t - 1 - off, w->cut)) {
    split_time = wmseed__div(t - off, w->cut) * w->cut + off;
  }

  while (w->sb->len && w->sb->sample_number <= w->sample_number) {
    sample = samplebuffer_pop(w->sb);
    tt = w->last_t + (int64_t) floor((w->sb->sample_number - w->last_sn - 1) * a);
    if (tt < w->end_time) {
      if ((!w->first_file_created && tt >= w->start_time) || tt >= split_time) {
        if (tt >= split_time) {
          split_time += w->cut;
        }
        if (tt >= w->start_time) {
          wmseed__create_file(w, tt);
          w->first_file_created = 1;
        }
      }
      if (w->first_file_created) {
        while (miniseed_record_push_sample(w->record, sample) == -1) {
          wmseed__new_record(w, tt);
        }
        w->data_pending = 1;
      }
    }
  }

  // Update state.
  w->last_t = t;
  w->last_sn = w->sample_number;

  return 0;
}

static void wmseed__resampler_callback(void *userdata, float *samples, int number_of_samples, int64_t start_time)
{
  WMSeed *w = userdata;
  wmseed__time(w, start_time);
  int i;
  for (i = 0; i < number_of_samples; ++i) {
    wmseed__sample(w, samples[i]);
  }
}

int wmseed_sample(WMSeed *w, int32_t sample)
{
  if (w->resampler) {
    resampler_sample(w->resampler, sample);
    return 0;
  } else {
    return wmseed__sample(w, sample);
  }
}

int wmseed_time(WMSeed *w, Time t)
{
  if (w->resampler) {
    resampler_time(w->resampler, t);
    return 0;
  } else {
    return wmseed__time(w, t);
  }
}

WMSeed *wmseed_new(FILE *logfile, const char *template, const char *station, const char *location, const char *channel, const char *network, double sample_rate, int64_t cut, int resampling)
{
  WMSeed *w;
  w = wmseed__allocate(0, 0, sizeof(*w));
  w->logfile = logfile;
  w->cut = cut * 1000000;
  w->sample_number = 0;
  w->template = wmseed__strdup(w, template);
  w->station = wmseed__strdup(w, station);
  w->location = wmseed__strdup(w, location);
  w->channel = wmseed__strdup(w, channel);
  w->network = wmseed__strdup(w, network);
  w->sample_rate = sample_rate;
  w->sb = samplebuffer_new();
  w->record_number = 0;
  w->record_time = 0;
  w->data_pending = 0;
  w->output = 0;
  w->last_t = 0;
  w->last_sn = -1;
  w->start_time = INT64_MIN;
  w->end_time = INT64_MAX;
  w->first_file_created = 0;
  if (resampling) {
    w->resampler = resampler_new(sample_rate, wmseed__resampler_callback, w);
    if (!w->resampler) {
      wmseed__log(w, stderr, "%s", i18n->out_of_memory);
      exit(1);
    }
  } else {
    w->resampler = 0;
  }
  return w;
}

#endif
