LIBS=-lutils -lcurl
CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g                                     

all: indexer

webpage.o: ../utils/webpage.c ../utils/webpage.h
				gcc $(CFLAGS) -c ../utils/webpage.c

indexer.o: indexer.c ../utils/webpage.h
				gcc $(CFLAGS) -c indexer.c

indexer:  indexer.o webpage.o
				gcc $(CFLAGS) indexer.c $(LIBS) -o $@                                               

clean:
				rm -f *.o indexer
