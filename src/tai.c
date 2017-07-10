#define TAI_IMPLEMENTATION
#include "tai.h"
#include <stdio.h>
#include <string.h>

#define leapyear(year) ((year) % 400 == 0 || ((year) % 4 == 0 && (year) % 100 != 0))
#define longmonth(month) ((((month) >> 3) ^ (month)) & 1)
#define next() c = (size ? (size--, *(input++)) : 0)
#define digit(c) ('0' <= (c) && (c) <= '9')
#define read_number(n) do { \
  if (!digit(c)) return -1; \
  (n) = c - '0'; \
  next(); \
  while (digit(c)) { \
    if ((n) >= 1000) return -1; \
    (n) = (n) * 10 + (c - '0'); \
    next(); \
  } \
} while (0)
int utc_parse(const char *input, int size, Date *utc)
{
  int year = 0, month = 1, day = 1, hour = 0, min = 0, sec = 0, c, t;
  long usec = 0;
  next();
  read_number(year);
  if (year > 4095) return -1;
  if (c != '-') return -1;
  next();
  read_number(month);
  if (month < 1 || 12 < month) return -1;
  if (c != '-') return -1;
  next();
  read_number(day);
  if (!(1 <= day && (day <= 28 || (leapyear(year) && day <= 29) ||
      (month != 2 && day <= 30) || (longmonth(month) && day <= 31))))
    return -1;
  t = 0;
  while (c == ' ' || c == 'T') {
    if (c == 'T') {
      t += 1;
      if (t > 1) return -1;
    }
    next();
  }
  if (!digit(c)) return -1;
  read_number(hour);
  if (hour > 23) return -1;
  if (c != ':') return -1;
  next();
  read_number(min);
  if (min > 59) return -1;
  if (c == 0) goto end;
  if (c == ' ' || c == 'Z' || c == 'U') goto suffix;
  if (c != ':') return -1;
  next();
  read_number(sec);
  if (sec > 60) return -1;
  if (c == 0) goto end;
  if (c == ' ' || c == 'Z' || c == 'U') goto suffix;
  if (c != '.' && c != ',') return -1;
  next();
  if (!digit(c)) return -1;
  usec += (c - '0') * 100000l;
  next();
  if (c == 0) goto end;
  if (c == ' ' || c == 'Z' || c == 'U') goto suffix;
  if (!digit(c)) return -1;
  usec += (c - '0') * 10000l;
  next();
  if (c == 0) goto end;
  if (c == ' ' || c == 'Z' || c == 'U') goto suffix;
  if (!digit(c)) return -1;
  usec += (c - '0') * 1000;
  next();
  if (c == 0) goto end;
  if (c == ' ' || c == 'Z' || c == 'U') goto suffix;
  if (!digit(c)) return -1;
  usec += (c - '0') * 100;
  next();
  if (c == 0) goto end;
  if (c == ' ' || c == 'Z' || c == 'U') goto suffix;
  if (!digit(c)) return -1;
  usec += (c - '0') * 10;
  next();
  if (c == 0) goto end;
  if (c == ' ' || c == 'Z' || c == 'U') goto suffix;
  if (!digit(c)) return -1;
  usec += (c - '0');
  next();
  while (digit(c)) {
    next();
  }
suffix:
  while (c == ' ') {
    next();
  }
  if (c == 0) goto end;
  if (c == 'Z') {
    next();
  } else if (c == 'U') {
    next();
    if (c != 'T') return -1;
    next();
    if (c != 'C') return -1;
    next();
  }
  if (c != 0) return -1;
end:
  utc->year = year;
  utc->month = month;
  utc->day = day;
  utc->hour = hour;
  utc->min = min;
  utc->sec = sec;
  utc->usec = usec;
  return 0;
}

static int time_parse(const char *input, int size, Time *t)
{
  Time tt = 0;
  int sign = 1, i;
  if (size > 1 && input[0] == '-') {
    sign = -1;
    ++input;
    --size;
  }
  for (i = 0; i < size; ++i) {
    if (input[i] < '0' || input[i] > '9' ||
      __builtin_mul_overflow(tt, 10, &tt) ||
      __builtin_add_overflow(tt, (input[i] - '0') * sign, &tt))
      return -1;
  }
  if (t) *t = tt;
  return 0;
}

int main(int argc, char **argv)
{
  int i;
  Date d;
  Time t;
  for (i = 1; i < argc; ++i) {
    if (utc_parse(argv[i], strlen(argv[i]), &d) == 0) {
      printf("%4d-%02d-%02d %02d:%02d:%02d UTC -> %lld\n",
        d.year, d.month, d.day,
        d.hour, d.min, d.sec,
        (long long) tai_time(d));
    } else if (time_parse(argv[i], strlen(argv[i]), &t) == 0) {
      d = tai_date(t, 0, 0);
      printf("%lld -> %4d-%02d-%02d %02d:%02d:%02d UTC\n",
        (long long) t,
        d.year, d.month, d.day,
        d.hour, d.min, d.sec);
    }
  }
}
