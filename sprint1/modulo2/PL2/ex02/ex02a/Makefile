prog: main.o
	gcc -m32 main.o -o prog

main.o: main.c
	gcc -m32 -g -Wall -c main.c -o main.o

clean:
	rm -rf *.o

run: prog
	./prog
