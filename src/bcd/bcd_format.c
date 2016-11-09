#include "bcd.h"
#include <stdio.h>

int bcd_format(const char *bcd, char *output, int maxlen)
{
  if (!bcd || !output || maxlen < 1) return -1;
  return snprintf(output, maxlen, "20%02d-%02d-%02d %02d:%02d:%02d UTC",
    bcd_int(bcd[BCD_YEAR]), bcd_int(bcd[BCD_MONTH]), bcd_int(bcd[BCD_DAY]),
    bcd_int(bcd[BCD_HOUR]), bcd_int(bcd[BCD_MINUTE]), bcd_int(bcd[BCD_SECOND]));
}
