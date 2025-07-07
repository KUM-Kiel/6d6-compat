#ifndef PROGRESS_H
#define PROGRESS_H

#include <stdio.h>

static void progress_update(long long done, long long total) {
  fprintf(stderr, "%5.1f%% %8.1fMB     \r", 100.0 * done / total, done * 1e-6);
  fflush(stderr);
}

static void progress_complete(long long total) {
  fprintf(stderr, "%5.1f%% %8.1fMB     \n", 100.0, total * 1e-6);
  fflush(stderr);
}

#endif
