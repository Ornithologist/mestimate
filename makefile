CC=gcc
CFLAGS=-g -O0 -fPIC -fno-builtin

all: check

default: check

clean:
	rm -rf *.o estimate

estimate: estimate.o
	$(CC) $(CFLAGS) $< -o $@

gdb: estimate
	gdb ./estimate

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

check:	estimate
	./estimate

dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar 