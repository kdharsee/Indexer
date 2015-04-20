CC=gcc
CFLAGS=-Wall -g -o 
XFLAGS=-lm
all:
	$(CC) $(CFLAGS) index index.c $(XFLAGS)
clean:
	rm -rf index
