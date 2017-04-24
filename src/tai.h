// TAI by Lukas Joeressen.
// Licensed under CC0-1.0.
// https://creativecommons.org/publicdomain/zero/1.0/legalcode
#ifndef TAI_INCLUDE
#define TAI_INCLUDE

#include <stdint.h>
#include <stddef.h>

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

int tai_utc_diff(Time t);

int tai_leapsecs_valid(Time t);
int tai_leapsecs_need_update(Time t);

#ifndef NO_STDIO
int tai_format(char *str, size_t size, Date date);
#endif

#endif

#ifdef TAI_IMPLEMENTATION
#undef TAI_IMPLEMENTATION

// b must be positive.
#define tai__div(a, b) ((a) / (b) - ((a) % (b) < 0))
#define tai__mod(a, b) (((a) % (b) + (b)) % (b))

static Time tai__leapsecs[] = {
  -867887989000000,
  -851990388000000,
  -820454387000000,
  -788918386000000,
  -757382385000000,
  -725759984000000,
  -694223983000000,
  -662687982000000,
  -631151981000000,
  -583891180000000,
  -552355179000000,
  -520819178000000,
  -457660777000000,
  -378691176000000,
  -315532775000000,
  -283996774000000,
  -236735973000000,
  -205199972000000,
  -173663971000000,
  -126230370000000,
  -78969569000000,
  -31535968000000,
  189388833000000,
  284083234000000,
  394416035000000,
  489024036000000,
  536544037000000,
};
static Time tai__last_valid = 568080037000000;
static Time tai__next_update = 554860837000000;

int tai_leapsecs_valid(Time t)
{
  return t < tai__last_valid;
}

int tai_leapsecs_need_update(Time t)
{
  return t > tai__next_update;
}

int tai_utc_diff(Time t)
{
  int i, d = 10;
  for (i = 0; i < sizeof(tai__leapsecs) / sizeof(*tai__leapsecs); ++i) {
    if (t < tai__leapsecs[i]) break;
    d += 1;
  }
  return d;
}

static Time tai__leapsec_add(Time t, int hit)
{
  int i;
  t += 10000000;
  for (i = 0; i < sizeof(tai__leapsecs) / sizeof(*tai__leapsecs); ++i) {
    if (t < tai__leapsecs[i] - 1000000) break;
    if (!hit || (t > tai__leapsecs[i])) t += 1000000;
  }
  return t;
}

static Time tai__leapsec_sub(Time t, int *hit)
{
  int i;
  Time s = 10000000;
  for (i = 0; i < sizeof(tai__leapsecs) / sizeof(*tai__leapsecs); ++i) {
    if (t < tai__leapsecs[i] - 1000000) break;
    s += 1000000;
    if (t < tai__leapsecs[i]) {
      if (hit) *hit = 1;
      return t - s;
    }
  }
  if (hit) *hit = 0;
  return t - s;
}

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

  return tai__leapsec_add(date.usec + 1000000 * (date.sec + 60 * (date.min + 60 * (date.hour + d * 24))), date.sec == 60);
}

Date tai_date(Time t, int *yday, int *wday)
{
  Date date;
  int64_t y, m;
  int yd, leap;

  t = tai__leapsec_sub(t, &leap);

  date.usec = tai__mod(t, 1000000);
  t = tai__div(t, 1000000);
  date.sec = tai__mod(t, 60) + leap;
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
#include <sys/time.h>
#include <unistd.h>
Time tai_now(void)
{
  Time t;
#ifdef CLOCK_REALTIME
  struct timespec tv;
#ifdef CLOCK_TAI
  if (clock_gettime(CLOCK_TAI, &tv) == 0) {
    t = (int64_t) (tv.tv_sec - 946684800) * 1000000 + tv.tv_nsec / 1000;
    // TODO: CLOCK_TAI is probably lying.
    return t;
  }
#endif
  if (clock_gettime(CLOCK_REALTIME, &tv) == 0) {
    t = (int64_t) (tv.tv_sec - 946684800) * 1000000 + tv.tv_nsec / 1000;
    t = tai__leapsec_add(t, 0);
    return t;
  }
#endif
  struct timeval tp;
  if (gettimeofday(&tp, 0) == 0) {
    t = (int64_t) (tp.tv_sec - 946684800) * 1000000 + tp.tv_usec;
    t = tai__leapsec_add(t, 0);
    return t;
  }
  return 0;
}

#ifndef NO_STDIO
#include <stdio.h>
int tai_format(char *str, size_t size, Date date)
{
  return snprintf(str, size,
    "%04d-%02d-%02d %02d:%02d:%02d.%06d",
    date.year, date.month, date.day,
    date.hour, date.min, date.sec, date.usec);
}
#endif

#endif
