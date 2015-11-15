CC=gcc
CFLAGS=-c -Wall -g

all: hw4

hw4: shell.o hw4.o
	$(CC) shell.o hw4.o -o hw4

shell.o: shell.c
	$(CC) $(CFLAGS) shell.c

hw3.o:	hw4.c
	$(CC) $(CFLAGS) hw4.c

clean:
	/bin/rm -f hw4 *.o *.gz

run:
	./hw4

tarball:

	tar -cvzf wooten.tar.gz *
