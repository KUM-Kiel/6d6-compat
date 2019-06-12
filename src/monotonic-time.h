#include <stdint.h>

#ifdef __APPLE__

// Mac OS
#include <mach/mach_time.h>
static inline int64_t monotonic_time(void)
{
  return mach_absolute_time() / 1000000;
}

static inline int64_t monotonic_time_ms(int ms)
{
  struct mach_timebase_info base[1];
  mach_timebase_info(base);
  return ms * (double) base->denom / base->numer;
}

#else

// Posix
#include <time.h>
static inline int64_t monotonic_time(void)
{
  struct timespec t[1];
  if (clock_gettime(CLOCK_MONOTONIC, t) < 0) return -1;
  return t->tv_sec * 1000 + t->tv_nsec / 1000000;
}

static inline int64_t monotonic_time_ms(int ms)
{
  return ms;
}

#endif
