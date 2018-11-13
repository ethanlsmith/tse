LIBS=-lutils -lcurl -lpthread
CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -lpthread  -g                             

all: lindexer

lindexer.o: lindexer.c
				gcc $(CFLAGS) -c lindexer.c

lindexer:  lindexer.o
				gcc $(CFLAGS) lindexer.c $(LIBS) -o $@                                               

clean:
				rm -f *.o lindexer
