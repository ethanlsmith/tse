CFLAGS=	-Wall -pedantic -std=c11  -I.	-g

all:	tlqueue

%.o:	%.c	%.h
							gcc	$(CFLAGS)	-c	queue.c

lqueue.o:	lqueue.c lqueue.h queue.h
							gcc $(CFLAGS) -c lqueue.c

tlqueue.o:	tlqueue.c	lqueue.h queue.h
							gcc	$(CFLAGS)	-c	tlqueue.c

tlqueue:		tlqueue.o	lqueue.o queue.o
							gcc	$(CFLAGS) -pthread	tlqueue.o	lqueue.o	queue.o -o	tlqueue

clean:
							rm	-f	*.o	tlqueue
