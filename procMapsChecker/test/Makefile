all: test

test: main.o libcheck.a
	gcc main.o -L../ -lcheck -o test

main.o: main.c
	gcc -c main.c -o main.o

libcheck.a:
	make -C ../

clean: FORCE
	rm *.o test

FORCE:
