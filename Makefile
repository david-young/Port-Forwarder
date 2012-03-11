# make for the client and servers

CC=gcc
CFLAGS=-Wall -lpthread -ggdb
EXEC=clnt svr_mt svr_s svr_ep

all: $(EXEC)

clnt: clnt.o
	$(CC) $(CFLAGS) clnt.o -o clnt

svr_mt: svr_mt.o
	$(CC) $(CFLAGS) svr_mt.o -o svr_mt

svr_s: svr_s.o
	$(CC) $(CFLAGS) svr_s.o -o svr_s

svr_ep: svr_ep.o
	$(CC) $(CFLAGS) svr_ep.o -o svr_ep

clean:
	rm -f *.o clnt svr_mt svr_ep svr_s core* *~

clnt.o: clnt.c
	$(CC) $(CFLAGS) -O -c clnt.c

svr_mt.o: svr_mt.c
	$(CC) $(CFLAGS) -O -c svr_mt.c

svr_s.o: svr_s.c
	$(CC) $(CFLAGS) -O -c svr_s.c

svr_ep.o: svr_ep.c
	$(CC) $(CFLAGS) -O -c svr_ep.c

