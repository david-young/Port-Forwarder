CC=gcc
CFLAGS=-Wall -pedantic -c -ggdb
CLIBS=-lpthread

all: svr

btree-test: binary-tree.o btree-test.o
	$(CC) $^ -o $@ $(CLIBS)

svr: svr.o helpers.o binary-tree.o 
	$(CC) $^ -o $@ $(CLIBS)

.c.o:
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.o btree-test svr core* *~

pkg: clean
	tar -cvzf COMP8005-FinalProject.tar.gz *
