DEBUG=-g3
PNG=-I/usr/include/libpng14  -lpng14
CFLAGS=-Wall
CC=gcc $(CFLAGS) $(DEBUG)

all: hide recover png_extractor png_restorer

stega.o: stega.c stega.h
	$(CC) -c $< -o $@

png_util.o: png_util.c png_util.h
	$(CC) -c $< -o $@ $(PNG)

file_utils.o: file_utils.c file_utils.h
	$(CC) -c $< -o $@

png_extractor: png_extractor.c png_util.o
	$(CC) $(PNG) $< -o $@ png_util.o -Wall

png_restorer: png_restorer.c  png_util.o
	$(CC) $(PNG) $< -o $@ png_util.o

hide: hide.c stega.o file_utils.o
	$(CC) -o $@ $< stega.o file_utils.o

recover:recover.c stega.o file_utils.o
	$(CC) $< -o $@ stega.o file_utils.o

.PHONY: clean

clean:
	rm stega.o file_utils.o png_util.o hide recover png_extractor png_restorer
