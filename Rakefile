require 'rake/c'

C.cflags = '-Wall -Os -m32'
C.ldflags = '-m32'

C.library '6d6', ['6d6.c', 'find_timestamp.c']
C.library 's2x', ['s2x.c', 's2x_channel.c']
C.library 'blockreader', ['blockreader.c']
C.library 'options', ['options.c']
C.library 'utime', ['utime.c']
C.library 'bcd', ['bcd_diff.c', 'bcd_mjd.c', 'bcd_parse.c', 'bcd_valid.c', 'bcd_weekday.c']

C.program '6d6copy', [
  '6d6copy.c', 'lib6d6.a', 'libblockreader.a', 'liboptions.a', '-lpthread', '-lm'
]

C.program '6d6read', [
  '6d6read.c', 'lib6d6.a', 'libs2x.a', 'libbcd.a', 'liboptions.a', '-lm'
]

C.program 's2xshift', [
  's2xshift.c', 'libs2x.a', '-lm'
]
