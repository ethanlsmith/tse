CFLAGS=	-Wall -pedantic -std=c11  -I.	-g

all:	tlqueue tlhash

queue.o:	queue.c	queue.h
							gcc	$(CFLAGS)	-c	queue.c

hash.o: hash.c hash.h
							gcc $(CFLAGS) -c hash.c

lqueue.o:	lqueue.c lqueue.h queue.h
							gcc $(CFLAGS) -c lqueue.c

lhash.o:		lhash.c lhash.h hash.h queue.h
							gcc $(CFLAGS) -c lhash.c

tlqueue.o:	tlqueue.c	lqueue.h queue.h
							gcc	$(CFLAGS)	-c	tlqueue.c

tlhash.o:		tlhash.c lhash.h hash.h queue.h
							gcc $(CFLAGS) -c tlhash.c

tlqueue:		tlqueue.o	lqueue.o queue.o
							gcc	$(CFLAGS) -pthread	tlqueue.o	lqueue.o queue.o -o	tlqueue

tlhash:			tlhash.o lhash.o hash.o queue.o
	gcc $(CFLAGS) -pthread tlhash.o lhash.o hash.o queue.o -o tlhash

clean:
							rm	-f	*.o	tlqueue
							rm	-f	*.o	tlhash
