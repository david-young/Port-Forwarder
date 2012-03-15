CC=gcc
CFLAGS=-Wall -pedantic -c -ggdb
CLIBS=-lpthread

all: binary-tree

binary-tree: binary-tree.o btree-test.o
	$(CC) $^ -o $@ $(CLIBS)

.c.o:
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.o binary-tree

pkg: clean
	tar -cvzf 8005-FinalProject.tar.gz *
