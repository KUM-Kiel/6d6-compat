sample-tracker-test.pdf: sample-tracker-test.R sample-tracker-test.c ../src/sample-tracker.h Makefile
	$(CC) -o sample-tracker-test sample-tracker-test.c -Os -Wall
	./sample-tracker-test > sample-tracker-test.csv
	Rscript sample-tracker-test.R
	rm -f sample-tracker-test sample-tracker-test.csv
