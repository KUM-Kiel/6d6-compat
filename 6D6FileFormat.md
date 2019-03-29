# 6D6 File Format Specifcation

## Description

The 6D6 file format consists of a series of bytes in a file or on a block
device.
The order of the bits in a byte is unspecified and the format is only based on
the byte level.

On the highest level a 6D6 file contains two 512-byte headers followed by a
series of data frames.
Data frames either contain sample data or metadata.

## Data Types

The 6D6 format makes use of different data types to represent its values.

In the description of the data types *mod* denotes the euclidean remainder operation and *floor(n)* denotes the largest integer which is not larger than *n*.

### Byte

This is the basic unit of access to the underlying file.
Each byte has a value between 0 and 255 inclusive.

### Text

Text consists of a series of bytes interpreted as UTF-8.
It may not contain a 0-byte or any sequences which are not valid UTF-8.
In some cases a text value will be terminated by 0-bytes.

### Uint16

This value consists of two consecutive bytes *b0* and *b1*.
Its value is calculated as *b0* × 256 + *b1* which makes it an unsigned big endian 16-bit integer.

### Uint32

This value consists of four consecutive bytes *b0*, *b1*, *b2*, and *b3*.
Its value is calculated as ((*b0* × 256 + *b1*) × 256 + *b2*) × 256 + *b3* which makes it an unsigned big endian 32-bit integer.

### Int32

This value consists of four consecutive bytes *b0*, *b1*, *b2*, and *b3*.
Its value is calculated as (((*b0* mod 128) × 256 + *b1*) × 256 + *b2*) × 256 + *b3* - 2147483648 × floor(*b0* ÷ 128) which makes it a signed big endian 32-bit integer.

### Uint64

This value consists of eight consecutive bytes *b0*-*b7*.
Its value is calculated as ((((((*b0* × 256 + *b1*) × 256 + *b2*) × 256 + *b3*) × 256 + *b4*) × 256 + *b5*) × 256 + *b6*) × 256 + *b7* which makes it an unsigned big endian 64-bit integer.

### BCD Value

A BCD value consists of a single byte *b0*.
Its value is calculated as (*b0* mod 16) + floor(*b0* ÷ 16) × 10.

### BCD Time

A BCD time consists of six consecutive BCD values *bcd0*-*bcd5* and denotes a date and time in the UTC timezone with the following components:

  - Year: *bcd5* + 2000
  - Month: *bcd4*
  - Day: *bcd3*
  - Hour: *bcd0*
  - Minute: *bcd1*
  - Second: *bcd2*

## 512-Byte Header

A 6D6 file starts with two 512-byte headers.
Each 512-byte header block consists of the following fields:

Name        | Bytes    | Data Type | Description
------------|----------|-----------|--------------------------------------------
—           | 4        | Text      | The exact text “time”.
time        | 6        | BCD Time  | UTC time of this header.
sync_type   | 4        | Text      | Type of the sync time.
sync_time   | 6        | BCD Time  | Time of synchronization.
skew        | 4        | Int32     | Difference in µs of UTC - internal clock at the time of synchronization.
—           | 4        | Text      | The exact text “addr”.
address     | 4        | Uint32    | Number of 512-byte blocks from the beginning of the file to the data.
—           | 4        | Text      | The exact text “rate”.
sample_rate | 2        | Uint16    | The sample rate in samples per second.
—           | 4        | Text      | The exact text “writ”.
written     | 8        | Uint64    | The number of sample frames written to the file.
—           | 4        | Text      | The exact text “lost”.
lost        | 4        | Uint32    | The number of samples lost during recording.
—           | 4        | Text      | The exact text “chan”.
channels    | 1        | Byte      | The number of channels in this recording.
—           | 4        | Text      | The exact text “gain”.
gain        | variable | Byte      | One byte of gain for each channel. Divide each byte by ten to get the real gain value.
—           | 4        | Text      | The exact text “bitd”.
bit_depth   | 1        | Byte      | The bit-depth of the recording.
—           | 4        | Text      | The exact text “rcid”.
recorder_id | variable | Text      | The serial number of the datalogger terminated by one or more 0-bytes.
—           | 4        | Text      | The exact text “rtci”.
rtc_id      | variable | Text      | The RTC serial number terminated by one or more 0-bytes.
—           | 4        | Text      | The exact text “lati”.
latitude    | variable | Text      | The latitude at the sync_time in text form terminated by one or more 0-bytes.
—           | 4        | Text      | The exact text “logi”.
longitude   | variable | Text      | The longitude at the sync_time in text form terminated by one or more 0-bytes.
—           | 4        | Text      | The exact text “alia”.
names       | variable | Text      | The channel names. Each name is terminated by exactly one 0-byte. There must be a name for each channel. After the last channel name there may be more than one 0-byte.
—           | 4        | Text      | The exact text “cmnt”.
comment     | variable | Text      | A free-form text comment for the recording.
—           | variable | Byte      | Zero padding to bring the header to a total length of 512 bytes.

Both headers have the same format but the interpretation of some values differs slightly depending on which header it is in.

Name      | Header | Interpretation
----------|--------|--------------------------------
time      | 1st    | Start time of the recording.
time      | 2nd    | End time of the recording.
sync_type | 1st    | Should be the exact text “sync”.
sync_type | 2nd    | Should be the exact text “skew” or four 0-bytes which indicate a missing second synchronization.
sync_time | 1st    | Time of first synchronization.
sync_time | 2nd    | Time of second synchronization.
skew      | 1st    | Clock difference at first synchronization.
skew      | 2nd    | Clock difference at second synchronization.
address   | 1st    | Beginning of the data.
address   | 2nd    | End of the data.
written   | 1st    | Must be 0.
written   | 2nd    | Number of samples written per channel.
lost      | 1st    | Must be 0.
lost      | 2nd    | Number of samples lost.
latitude  | 1st    | Latitude at first synchronization.
latitude  | 2nd    | Latitude at second synchronization.
longitude | 1st    | Longitude at first synchronization.
longitude | 2nd    | Longitude at second synchronization.

All other fields should be the same for both headers.

## Data Frames

After the two headers follow the data frames.
Each data frame is either a sample frame or a metadata frame.

All data frames start with an Int32.
If this Int32 is even the data frame is a sample frame.
Otherwise it is a metadata frame.

Sample frames consist of one Int32 for every channel, i.e. in a four channel recording each sample frame is 16 bytes long.
All samples must be even and the order is the same as the names and gains in the header.

A metadata frame is 16 bytes long regardless of the number of channels.
The interpretation of the remaining 12 bytes depends on the value of the first Int32.
If a metadata frame uses less than 12 bytes of payload data, the remaining bytes are padded with zeros.

The different metadata frames are described below.
More metadata frames may be added in the future.

### 1: Timestamp

A timestamp frame contains a time in µs relative to the time field of the first header.
This time is the time of the next sample frame.

Field   | Bytes | Data Type | Description
--------|-------|-----------|---------------
id      | 4     | Int32     | The value 1.
sec     | 4     | Uint32    | Seconds.
usec    | 4     | Uint32    | Microseconds (0-999999).
padding | 4     | Byte      | 0-bytes.

### 3: Voltage & Humidity

Field    | Bytes | Data Type | Description
---------|-------|-----------|---------------
id       | 4     | Int32     | The value 3.
voltage  | 2     | Uint16    | The battery voltage in units of 0.01V.
humidity | 2     | Uint16    | The relative humidity in %.
padding  | 8     | Byte      | 0-bytes.

### 5: Temperature

Field       | Bytes | Data Type | Description
------------|-------|-----------|---------------
id          | 4     | Int32     | The value 5.
temperature | 2     | Int16     | The temperature in units of 0.01°C.
padding     | 10    | Byte      | 0-bytes.

### 7: Lost samples indicator

This metadata frame indicates that samples have been lost at this position.

Field    | Bytes | Data Type | Description
---------|-------|-----------|---------------
id       | 4     | Int32     | The value 7.
time     | 6     | BCD Time  | The time of the loss.
samples  | 4     | Uint32    | The number of lost samples.
padding  | 2     | Byte      | 0-bytes.

### 9: Recording ID

This metadata frame contains the start time as an integrity check.

Field    | Bytes | Data Type | Description
---------|-------|-----------|---------------
id       | 4     | Int32     | The value 9.
time     | 6     | BCD Time  | The time field of the first header.
padding  | 6     | Byte      | 0-bytes.

### 11: Reboot indicator

This metadata frame indicates that the datalogger was reset at this position and continued the recording.

Field    | Bytes | Data Type | Description
---------|-------|-----------|---------------
id       | 4     | Int32     | The value 11.
time     | 6     | BCD Time  | The time of the reboot.
voltage  | 2     | Uint16    | The battery voltage after the reboot in units of 0.01V.
padding  | 4     | Byte      | 0-bytes.

### 13: End of recording indicator

This metadata frame indicates the end of the recording.

Field    | Bytes | Data Type | Description
---------|-------|-----------|---------------
id       | 4     | Int32     | The value 13.
time     | 6     | BCD Time  | The time field of the second header.
padding  | 2     | Byte      | 0-bytes.
