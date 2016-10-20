#pragma once

#include <stdint.h>

typedef int64_t Time;

extern Time time_now(void);
extern Time time_utc(int year, int month, int day, int hour, int min, int sec, int usec);
