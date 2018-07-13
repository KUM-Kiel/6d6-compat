# Frequently Asked Questions

## What does 6d6mseed do with my data?

`6d6mseed` takes the raw sample data and puts it into MiniSEED files.

Each MiniSEED file consists of a number of records, which contain about 1000
samples each and the following information.

  - Record Number
  - Station Code (5 Characters, specified on command line)
  - Location Identifier (2 Characters, specified on command line)
  - Channel Name (3 Characters, specified on command line)
  - Network Code (2 Characters, specified on command line)
  - Record Start Time
  - Time Correction (always zero)
  - Sample Rate

The record start time is determined in a best effort principle using the skew
measurement and the timestamps in the data.

The sample rate is the nominal sample rate. The actual sample rate might be
slightly different due to the skew.

Cuts between the MiniSEED files are made according to the final corrected times.
This means that your day file at 100 samples per second may have slightly more
or less than 8640000 samples.
