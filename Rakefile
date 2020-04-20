require 'rake/c'

machine = `#{C.cc} -dumpmachine`.strip
strip = ENV['STRIP'] || 'strip'

if machine.match(/darwin/)
  C.cflags = '-Wall -Os'
  host = "mac"
else
  C.cflags = '-Wall -Os'
  C.ldflags = '-Os'
  host = "linux"
end

C.library 'tai', [
  'tai.c',
]

C.library 'i18n', [
  'i18n.c',
  'i18n_error.c',
]

C.library '6d6', [
  '6d6.c',
  'find_timestamp.c',
]

C.library 's2x', [
  's2x.c',
  's2x_channel.c',
]

C.library 'blockreader', [
  'blockreader.c',
]

C.library 'options', [
  'options.c',
]

C.library 'utime', [
  'utime.c',
]

C.library 'bcd', [
  'bcd_diff.c',
  'bcd_mjd.c',
  'bcd_parse.c',
  'bcd_valid.c',
  'bcd_weekday.c',
  'bcd_format.c',
]

C.library 'samplerate', [
  'samplerate.c',
  'src_linear.c',
  'src_sinc.c',
  'src_zoh.c',
]

C.program '6d6copy', [
  '6d6copy.c',
  'lib6d6.a',
  'liboptions.a',
  'libbcd.a',
  'libtai.a',
  'libi18n.a',
  '-lm',
]

C.program '6d6read', [
  '6d6read.c',
  'lib6d6.a',
  'libs2x.a',
  'libbcd.a',
  'libtai.a',
  'liboptions.a',
  'libi18n.a',
  '-lm',
]

C.program '6d6info', [
  '6d6info.c',
  'lib6d6.a',
  'liboptions.a',
  'libbcd.a',
  'libtai.a',
  'libi18n.a',
  '-lm',
]

C.program '6d6mseed', [
  '6d6mseed.c',
  'lib6d6.a',
  'liboptions.a',
  'libbcd.a',
  'libtai.a',
  'libi18n.a',
  'libsamplerate.a',
  '-lm',
]

C.program 's2xshift', [
  's2xshift.c',
  'libs2x.a',
  '-lm',
]

C.program 's2xdump', [
  's2xdump.c',
  'libs2x.a',
  '-lm',
]

C.program 'tai', [
  'tai.c',
  'libtai.a',
]

C.program 'mseed-check', [
  'mseed-check.c',
  'libtai.a',
]

require './i18n'
file 'src/i18n/i18n.h' => ['i18n/en_GB.txt', 'i18n/de_DE.txt', 'i18n.rb'] do
  i18n = I18n.new
  i18n.add_lang 'en_GB', 'i18n/en_GB.txt'
  i18n.add_lang 'de_DE', 'i18n/de_DE.txt'
  system 'mkdir -p src/i18n/'
  i18n.write_h 'src/i18n/i18n.h'
end

desc "Install everything."
task :install => ['build/6d6info', 'build/6d6copy', 'build/6d6read', 'build/6d6mseed'] do
  #system 'strip build/6d6info build/6d6copy build/6d6read build/6d6mseed'
  system 'sudo install -m 4755 "build/6d6info" "/usr/local/bin/"'
  system 'sudo install -m 4755 "build/6d6copy" "/usr/local/bin/"'
  system 'sudo install -m 0755 "build/6d6read" "/usr/local/bin/"'
  system 'sudo install -m 4755 "build/6d6mseed" "/usr/local/bin/"'
end

desc "Create a packaged version."
task :package => [
  'build/6d6info', 'build/6d6copy', 'build/6d6read', 'build/6d6mseed',
  'src/version.h', 'package/install', 'package/README', 'package/6d6update', 'LICENCE'
] do
  v = File.read 'src/version.h'
  version = v.match(/KUM_6D6_COMPAT_VERSION\s+"([^"]+)"/)[1]
  date = v.match(/KUM_6D6_COMPAT_DATE\s+"([^"]+)"/)[1]
  archive = "6d6-compat-#{date}-#{version}-#{host}"
  system "rm -rf '#{archive}' '#{archive}.tar.gz'"
  system "mkdir '#{archive}'"
  system "cp build/6d6info build/6d6copy build/6d6read build/6d6mseed package/6d6update package/install package/README LICENCE '#{archive}'"
  system "#{strip} '#{archive}/6d6info'"
  system "#{strip} '#{archive}/6d6copy'"
  system "#{strip} '#{archive}/6d6read'"
  system "#{strip} '#{archive}/6d6mseed'"
  system "tar czf '#{archive}.tar.gz' '#{archive}'"
end
