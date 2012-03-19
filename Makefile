CC=gcc
CFLAGS=-Wall -pedantic -c -ggdb
CLIBS=-lpthread

all: binary-tree svr

binary-tree: binary-tree.o btree-test.o
	$(CC) $^ -o $@ $(CLIBS)

svr: svr.o binary-tree.o
	$(CC) $^ -o $@ $(CLIBS)

.c.o:
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.o binary-tree svr core* *~

pkg: clean
	tar -cvzf c8005-FinalProject.tar.gz *
