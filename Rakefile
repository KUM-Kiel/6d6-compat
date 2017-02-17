require 'rake/c'

C.cflags = '-Wall -Os -m32 -static'
C.ldflags = '-m32 -static'

C.library '6d6', [
  '6d6.c',
  'find_timestamp.c'
]

C.library 's2x', [
  's2x.c',
  's2x_channel.c'
]

C.library 'blockreader', [
  'blockreader.c'
]

C.library 'options', [
  'options.c'
]

C.library 'utime', [
  'utime.c'
]

C.library 'bcd', [
  'bcd_diff.c',
  'bcd_mjd.c',
  'bcd_parse.c',
  'bcd_valid.c',
  'bcd_weekday.c',
  'bcd_format.c'
]

C.library 'samplerate', [
  'samplerate.c',
  'src_linear.c',
  'src_sinc.c',
  'src_zoh.c'
]

C.program '6d6copy', [
  '6d6copy.c',
  'lib6d6.a',
  'liboptions.a',
  '-lm'
]

C.program '6d6read', [
  '6d6read.c',
  'lib6d6.a',
  'libs2x.a',
  'libbcd.a',
  'liboptions.a',
  '-lm'
]

C.program '6d6info', [
  '6d6info.c',
  'lib6d6.a',
  'liboptions.a',
  'libbcd.a',
  '-lm'
]

C.program '6d6mseed', [
  '6d6mseed.c',
  'lib6d6.a',
  'liboptions.a',
  'libbcd.a',
  '-lm'
]

C.program 's2xshift', [
  's2xshift.c',
  'libs2x.a',
  '-lm'
]

desc "Install everything."
task :install => ['build/6d6info', 'build/6d6copy', 'build/6d6read', 'build/6d6mseed'] do
  system 'strip build/6d6info build/6d6copy build/6d6read build/6d6mseed'
  system 'sudo install -m 4755 "build/6d6info" "/usr/local/bin/"'
  system 'sudo install -m 4755 "build/6d6copy" "/usr/local/bin/"'
  system 'sudo install -m 0755 "build/6d6read" "/usr/local/bin/"'
  system 'sudo install -m 4755 "build/6d6mseed" "/usr/local/bin/"'
end

desc "Create a packaged version."
task :package => [
  'build/6d6info', 'build/6d6copy', 'build/6d6read', 'build/6d6mseed',
  'src/version.h', 'package/Makefile', 'package/README', 'LICENSE'
] do
  v = File.read 'src/version.h'
  version = v.match(/KUM_6D6_COMPAT_VERSION\s+"([^"]+)"/)[1]
  date = v.match(/KUM_6D6_COMPAT_DATE\s+"([^"]+)"/)[1]
  archive = "6d6-compat-#{date}-#{version}-linux-i386"
  system 'strip build/6d6info build/6d6copy build/6d6read build/6d6mseed'
  system "rm -rf '#{archive}'"
    system "mkdir '#{archive}'"
  system "cp build/6d6info build/6d6copy build/6d6read build/6d6mseed package/Makefile package/README LICENSE '#{archive}'"
  system "tar czf '#{archive}.tar.gz' '#{archive}'"
end
