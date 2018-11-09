LIBS=-lutils -lcurl
CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g                                     

all: tpageio

webpage.o: ../utils/webpage.c ../utils/webpage.h
				gcc $(CFLAGS) -c ../utils/webpage.c

tpageio.o: tpageio.c ../utils/webpage.h
				gcc $(CFLAGS) -c tpageio.c

tpageio:  tpageio.o webpage.o
				gcc $(CFLAGS) tpageio.c $(LIBS) -o $@                                               

clean:
				rm -f *.o tpageio
