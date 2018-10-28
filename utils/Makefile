CFLAGS=-Wall -pedantic -std=c11 -I. -g
OFILES=queue.o hash.o webpage.o

all:	$(OFILES)
			ar cr ../lib/libutils.a $(OFILES) 

webpage.o:	webpage.h webpage.c
						gcc $(CFLAGS) -c webpage.c 

clean:
							rm	-f webpage.o
