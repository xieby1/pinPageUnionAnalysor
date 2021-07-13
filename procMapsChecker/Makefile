CC = gcc
CFLAGS =

all: libcheck.a

libcheck.a: check.o pmparser.o
	ar rcs $@ $^

%.o: %.c %.h
	${CC} -c ${CFLAGS} -o $@ $<

pmparser.o: proc_maps_parser/pmparser.c proc_maps_parser/pmparser.h
	${CC} -c ${CFLAGS} -o $@ $<

clean: FORCE
	rm -f *.o *.a

FORCE:
