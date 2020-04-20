cc resampler-test.c ../build/libsamplerate.a -I../src/samplerate/ -Wall -Wfatal-errors -Werror -Os -lm -o resampler-test
./resampler-test > resampler-test.csv
Rscript resampler-test.R
