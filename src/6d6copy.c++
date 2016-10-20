#define _FILE_OFFSET_BITS 64
#include <cstdio>
#include <cstdlib>
#include "options.h"

int main(int argc, char **argv)
{
  int progress = 1;
  Options o;
  o.flag('h', "help", [&]() {
    fprintf(stderr,
      "6D6 Compatibility Tools %s\n"
      "(c) 2016 KUM Kiel GmbH\n"
      "\n"
      "# Usage\n"
      "\n"
      "  %s [options] /dev/sdX1 out.6d6\n"
      "\n"
      "  This copies all the data from the SD card or StiK to the specified\n"
      "  output file.\n"
      "\n"
      "# Options\n"
      "\n"
      "  -h, --help\n"
      "  Show this help text and exit.\n"
      "\n"
      "  --no-progress\n"
      "  Disable the progress output.\n"
      "\n",
      6D6COMPAT_VERSION,
      argv[0]);
    exit(1);
  });
  o.flag(0, "no-progress", [&]() {
    progress = 0;
  });
  o.parse(argc, argv);

  return 0;
}
