# make for the client and servers

CC=gcc
CFLAGS=-Wall -lpthread -ggdb
EXEC=svr

all: $(EXEC)

svr: svr.o
	$(CC) $(CFLAGS) svr.o -o svr

clean:
	rm -f *.o svr core* *~

svr.o: svr.c
	$(CC) $(CFLAGS) -O -c svr.c

