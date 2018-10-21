LIBS=-lutils -lcurl
CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g

all: crawler

webpage.o: ../utils/webpage.c ../utils/webpage.h
			gcc $(CFLAGS) -c ../utils/webpage.c

crawler.o: crawler.c ../utils/webpage.h
					gcc $(CFLAGS) -c crawler.c

crawler:	crawler.o	webpage.o
				gcc $(CFLAGS) crawler.c $(LIBS) -o $@

clean:
				rm -f *.o crawler
