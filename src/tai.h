// TAI by Lukas Joeressen.
// Licensed under CC0-1.0.
// https://creativecommons.org/publicdomain/zero/1.0/legalcode
#ifndef TAI_INCLUDE
#define TAI_INCLUDE

#include <stdint.h>

typedef int64_t Time;
typedef struct {
  int year, month, day;
  int hour, min, sec;
  int usec;
} Date;

// Convert a TAI date into a timestamp.
Time tai_time(Date date);

Date tai_date(Time t, int *yday, int *wday);

Time tai_now(void);

#ifndef NO_STDIO
int tai_format(char *str, size_t size, Date date);
#endif

#endif

#ifdef TAI_IMPLEMENTATION
#undef TAI_IMPLEMENTATION

// b must be positive.
#define tai__div(a, b) ((a) / (b) - ((a) % (b) < 0))
#define tai__mod(a, b) (((a) % (b) + (b)) % (b))

Time tai_time(Date date)
{
  int64_t y, m, d, t;
  y = date.year - 2000;
  m = date.month - 3;
  d = date.day + 59;

  t = tai__div(m, 12);
  m -= 12 * t;
  y += t;

  d += (m * 153 + 2) / 5;
  d += y * 365 + tai__div(y, 4) - tai__div(y, 100) + tai__div(y, 400);

  return date.usec + 1000000 * (date.sec + 60 * (date.min + 60 * (date.hour + d * 24)));
}

Date tai_date(Time t, int *yday, int *wday)
{
  Date date;
  int64_t y, m;
  int yd;
  date.usec = tai__mod(t, 1000000);
  t = tai__div(t, 1000000);
  date.sec = tai__mod(t, 60);
  t = tai__div(t, 60);
  date.min = tai__mod(t, 60);
  t = tai__div(t, 60);
  date.hour = tai__mod(t, 24);
  t = tai__div(t, 24);

  y = t / 146097;
  t %= 146097;
  t += 730425;
  while (t >= 146097) {
    t -= 146097;
    ++y;
  }

  if (wday) *wday = (t + 3) % 7;

  y *= 4;
  if (t == 146096) {
    y += 3;
    t = 36524;
  } else {
    y += t / 36524;
    t %= 36524;
  }
  y *= 25;
  y += t / 1461;
  t %= 1461;
  y *= 4;

  yd = (t < 306);
  if (t == 1460) {
    y += 3;
    t = 365;
  } else {
    y += t / 365;
    t %= 365;
  }
  yd += t;

  t *= 10;
  m = (t + 5) / 306;
  t = (t + 5) % 306;
  t /= 10;
  if (m >= 10) {
    yd -= 306;
    ++y;
    m -= 10;
  } else {
    yd += 59;
    m += 2;
  }

  date.year = y;
  date.month = m + 1;
  date.day = t + 1;

  if (yday) *yday = yd;

  return date;
}

#include <time.h>
Time tai_now(void)
{
  struct timespec tv;
  clock_gettime(CLOCK_TAI, &tv);
  return (int64_t) (tv.tv_sec - 946684800) * 1000000 + tv.tv_nsec / 1000;
}

#ifndef NO_STDIO
#include <stdio.h>
int tai_format(char *str, size_t size, Date date)
{
  return snprintf(str, size,
    "%04d-%02d-%02d %02d:%02d:%02d.%06d TAI",
    date.year, date.month, date.day,
    date.hour, date.min, date.sec, date.usec);
}
#endif

#endif
