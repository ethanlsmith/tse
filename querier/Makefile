CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g

all: query

query.o: query.c
					gcc $(CFLAGS) -c query.c

query: query.o
					gcc $(CFLAGS) query.c -o $@

clean:
					rm -f *.o query
