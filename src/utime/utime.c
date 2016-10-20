#include "utime.h"

#include <sys/time.h>

#define SEC 1000000ll
#define DAY (86400ll * SEC)

static int times365[4] = {0, 365, 730, 1095};
static long times36524[4] = {0, 36524l, 73048l, 109572l};
static int montab[12] = {0, 31, 61, 92, 122, 153, 184, 214, 245, 275, 306, 337};
/* month length after february is (306 * m + 5) / 10 */

static int64_t mjd(int64_t y, int64_t m, int64_t d)
{
  d -= 678882l;
  m -= 1;

  d += 146097l * (y / 400);
  y %= 400;

  if (m >= 2) m -= 2; else { m += 10; --y; }

  y += (m / 12);
  m %= 12;
  if (m < 0) { m += 12; --y; }

  d += montab[m];

  d += 146097l * (y / 400);
  y %= 400;
  if (y < 0) { y += 400; d -= 146097l; }

  d += times365[y & 3];
  y >>= 2;

  d += 1461 * (y % 25);
  y /= 25;

  d += times36524[y & 3];

  return d;
}

Time time_now(void)
{
  Time t;
  struct timeval tv;
  gettimeofday(&tv, 0);
  t = tv.tv_sec * SEC + tv.tv_usec;
  return t;
}

Time time_utc(int year, int month, int day, int hour, int min, int sec, int usec)
{
  Time t = (mjd(year, month, day) - 40587) * DAY + ((hour * 60 + min) * 60 + sec) * SEC + usec;
  return t;
}
