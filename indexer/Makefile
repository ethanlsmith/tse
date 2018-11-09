LIBS=-lutils -lcurl
CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g                                     

all: indexer

indexer.o: indexer.c
				gcc $(CFLAGS) -c indexer.c

indexer:  indexer.o
				gcc $(CFLAGS) indexer.c $(LIBS) -o $@                                               

clean:
				rm -f *.o indexer
