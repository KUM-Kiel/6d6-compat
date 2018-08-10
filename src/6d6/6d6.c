#include "6d6.h"

#include <string.h>
#include <inttypes.h>
#include "number.h"
#include "bcd.h"
#include "tai.h"
#include "i18n.h"

#define X ((const uint8_t *) x)
#define SX ((const char *) x)
#define ADV_O(a) o += (a); if (o >= 512) return -1
#define SKIP0() while (!X[o]) { ADV_O(1); }

int kum_6d6_header_read(kum_6d6_header *header, const void *x)
{
  int o = 0, n, i, nc;
  if (!header || !x) return -1;
  /* The last byte must be zero. */
  if (X[511] != 0) return -1;
  /* Version */
  if (memcmp(X + o, "6D6\002", 4) == 0) {
    header->version = 2;
    ADV_O(4);
  } else {
    header->version = 1;
  }
  /* Time */
  if (memcmp(X + o, "time", 4)) return -1;
  ADV_O(4);
  memcpy(header->start_time, X + o, 6);
  ADV_O(6);
  /* Sync/Skew */
  if (!memcmp(X + o, "sync", 4) && bcd_valid(SX + o + 4)) {
    header->sync_type = KUM_6D6_SYNC;
    memcpy(header->sync_time, X + o + 4, 6);
    header->skew = ld_i32_be(X + o + 10);
  } else if (!memcmp(X + o, "skew", 4) && bcd_valid(SX + o + 4)) {
    header->sync_type = KUM_6D6_SKEW;
    memcpy(header->sync_time, X + o + 4, 6);
    header->skew = ld_i32_be(X + o + 10);
  } else {
    header->sync_type = KUM_6D6_NONE;
    memset(header->sync_time, 0, 6);
    header->skew = 0;
  }
  ADV_O(14);
  /* Address */
  if (memcmp(X + o, "addr", 4)) return -1;
  ADV_O(4);
  header->address = ld_u32_be(X + o);
  ADV_O(4);
  /* Sample Rate */
  if (memcmp(X + o, "rate", 4)) return -1;
  ADV_O(4);
  header->sample_rate = ld_u16_be(X + o);
  ADV_O(2);
  /* Written Samples */
  if (memcmp(X + o, "writ", 4)) return -1;
  ADV_O(4);
  header->written_samples = ld_u64_be(X + o);
  ADV_O(8);
  /* Lost Samples */
  if (memcmp(X + o, "lost", 4)) return -1;
  ADV_O(4);
  header->lost_samples = ld_u32_be(X + o);
  ADV_O(4);
  /* Channel Count */
  if (memcmp(X + o, "chan", 4)) return -1;
  ADV_O(4);
  nc = header->channel_count = ld_u8_be(X + o);
  if (nc < 1 || nc > KUM_6D6_MAX_CHANNEL_COUNT) return -1;
  ADV_O(1);
  /* Gain */
  if (memcmp(X + o, "gain", 4)) return -1;
  ADV_O(4);
  memcpy(header->gain, X + o, nc);
  if (nc < KUM_6D6_MAX_CHANNEL_COUNT) {
    memset(header->gain + nc, 0, KUM_6D6_MAX_CHANNEL_COUNT - nc);
  }
  ADV_O(nc);
  /* Bit Depth */
  if (memcmp(X + o, "bitd", 4)) return -1;
  ADV_O(4);
  header->bit_depth = ld_u8_be(X + o);
  ADV_O(1);
  /* Recorder ID */
  if (memcmp(X + o, "rcid", 4)) return -1;
  ADV_O(4);
  n = strlen(SX + o);
  if (n >= sizeof(header->recorder_id)) return -1;
  if (n) memcpy(header->recorder_id, X + o, n);
  memset(header->recorder_id + n, 0, sizeof(header->recorder_id) - n);
  ADV_O(n + 1);
  SKIP0();
  /* RTC ID */
  if (memcmp(X + o, "rtci", 4)) return -1;
  ADV_O(4);
  n = strlen(SX + o);
  if (n >= sizeof(header->rtc_id)) return -1;
  if (n) memcpy(header->rtc_id, X + o, n);
  memset(header->rtc_id + n, 0, sizeof(header->rtc_id) - n);
  ADV_O(n + 1);
  SKIP0();
  /* Latitude */
  if (memcmp(X + o, "lati", 4)) return -1;
  ADV_O(4);
  n = strlen(SX + o);
  if (n >= sizeof(header->latitude)) return -1;
  if (n) memcpy(header->latitude, X + o, n);
  memset(header->latitude + n, 0, sizeof(header->latitude) - n);
  ADV_O(n + 1);
  SKIP0();
  /* Longitude */
  if (memcmp(X + o, "logi", 4)) return -1;
  ADV_O(4);
  n = strlen(SX + o);
  if (n >= sizeof(header->longitude)) return -1;
  if (n) memcpy(header->longitude, X + o, n);
  memset(header->longitude + n, 0, sizeof(header->longitude) - n);
  ADV_O(n + 1);
  SKIP0();
  /* Channel Names */
  if (memcmp(X + o, "alia", 4)) return -1;
  ADV_O(4);
  for (i = 0; i < nc; ++i) {
    n = strlen(SX + o);
    if (n >= sizeof(header->channel_names[i])) return -1;
    if (n) memcpy(header->channel_names[i], X + o, n);
    memset(header->channel_names[i] + n, 0, sizeof(header->channel_names[i]) - n);
    ADV_O(n + 1);
  }
  SKIP0();
  /* Comment */
  if (memcmp(X + o, "cmnt", 4)) return -1;
  ADV_O(4);
  n = strlen(SX + o);
  if (n >= sizeof(header->comment)) return -1;
  if (n) memcpy(header->comment, X + o, n);
  memset(header->comment + n, 0, sizeof(header->comment) - n);
  return 0;
}

static int format_duration(long d, char *out, int maxlen)
{
  int l, n = 0;
  long t;
  t = d / 86400;
  if (t) {
    l = snprintf(out, maxlen, "%ldd", t);
    if (l < 0) return l;
    n += l;
  }
  d %= 86400;
  t = d / 3600;
  if (t) {
    l = snprintf(out + n, maxlen - n, "%s%ldh", n ? " " : "", t);
    if (l < 0) return l;
    n += l;
  }
  d %= 3600;
  t = d / 60;
  if (t) {
    l = snprintf(out + n, maxlen - n, "%s%ldm", n ? " " : "", t);
    if (l < 0) return l;
    n += l;
  }
  d %= 60;
  if (d) {
    l = snprintf(out + n, maxlen - n, "%s%lds", n ? " " : "", d);
    if (l < 0) return l;
    n += l;
  }
  return n;
}

static void print_leftpad(FILE *f, const char *s, const char *pad)
{
  int lf = 0;
  while (*s) {
    if (lf && *s != '\n') {
      fputs(pad, f);
      lf = 0;
    }
    if (*s == '\n') {
      lf = 1;
    }
    fputc(*s, f);
    ++s;
  }
  if (!lf) {
    fputc('\n', f);
  }
}

static Time bcd_time(const uint8_t *bcd)
{
  Date date = {
    .year = bcd_int(bcd[BCD_YEAR]) + 2000,
    .month = bcd_int(bcd[BCD_MONTH]),
    .day = bcd_int(bcd[BCD_DAY]),
    .hour = bcd_int(bcd[BCD_HOUR]),
    .min = bcd_int(bcd[BCD_MINUTE]),
    .sec = bcd_int(bcd[BCD_SECOND]),
    .usec = 0
  };
  return tai_time(date);
}

int kum_6d6_show_info(FILE *f, kum_6d6_header *start_header, kum_6d6_header *end_header)
{
  char buffer[128];
  Time start_time, end_time, sync_time, skew_time = 0;
  Date d;
  double skew = 0;
  int i;

  if (start_header->sync_type != KUM_6D6_SYNC) return -1;
  /* Calculate times. */
  sync_time = bcd_time(start_header->sync_time);
  start_time = bcd_time(start_header->start_time);
  end_time = bcd_time(end_header->start_time);
  /* Leap second between sync and start end? */
  start_time += 1000000 * (tai_utc_diff(start_time) - tai_utc_diff(sync_time));
  end_time += 1000000 * (tai_utc_diff(end_time) - tai_utc_diff(sync_time));
  if (end_header->sync_type == KUM_6D6_SKEW) {
    skew_time = bcd_time(end_header->sync_time);
    end_header->skew += 1000000 * (tai_utc_diff(skew_time) - tai_utc_diff(sync_time));
    skew = 1e6 * (end_header->skew - start_header->skew) / (skew_time - sync_time);
    skew_time += 1000000 * (tai_utc_diff(skew_time) - tai_utc_diff(sync_time));
  }

  /* Show all the info. */
  fprintf(f, "%s %s\n", i18n->label_6d6_sn, start_header->recorder_id);
  d = tai_date(start_time, 0, 0);
  fprintf(f, "%s %4d-%02d-%02d %02d:%02d:%02d UTC\n",
    i18n->label_start_time,
    d.year, d.month, d.day, d.hour, d.min, d.sec);
  d = tai_date(end_time, 0, 0);
  fprintf(f, "%s %4d-%02d-%02d %02d:%02d:%02d UTC\n",
    i18n->label_end_time,
    d.year, d.month, d.day, d.hour, d.min, d.sec);
  d = tai_date(sync_time, 0, 0);
  fprintf(f, "%s %4d-%02d-%02d %02d:%02d:%02d UTC\n",
    i18n->label_sync_time,
    d.year, d.month, d.day, d.hour, d.min, d.sec);
  if (end_header->sync_type == KUM_6D6_SKEW) {
    d = tai_date(skew_time, 0, 0);
    fprintf(f, "%s %4d-%02d-%02d %02d:%02d:%02d UTC\n",
      i18n->label_skew_time,
      d.year, d.month, d.day, d.hour, d.min, d.sec);
    fprintf(f, "%s %" PRId64 "µs (%.3fppm)\n",
      i18n->label_skew,
      end_header->skew, skew);
  }
  format_duration(bcd_diff((char *) start_header->start_time, (char *) end_header->start_time), buffer, sizeof(buffer));
  fprintf(f, "%s %s\n",
    i18n->label_duration, buffer);
  fprintf(f, "%s %d SPS\n",
    i18n->label_sample_rate, start_header->sample_rate);
  // Create padding label.
  for (i = 0; i < strlen(i18n->label_blank); ++i) {
    buffer[i] = ' ';
  }
  buffer[i] = 0;
  for (i = 0; i < start_header->channel_count; ++i) {
    fprintf(f, "%s %s (%s %.1f)\n",
      i ? buffer : i18n->label_channels,
      start_header->channel_names[i],
      i18n->gain, start_header->gain[i] / 10.0);
  }
  fprintf(f, "%s %.1f MB\n",
    i18n->label_size, end_header->address * 512.0 / 1e6);

  fprintf(f, "%s ", i18n->label_comment);
  for (i = 0; i < strlen(i18n->label_blank) + 1; ++i) {
    buffer[i] = ' ';
  }
  buffer[i] = 0;
  print_leftpad(f, (char *) start_header->comment, buffer);

  return 0;
}
