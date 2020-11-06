main.o: main.c header.h
	gcc -lrt -lm -Wall -g -c main.c

main: main.o
	gcc -lrt -lm -Wall -g -o main main.o

run: main
	rm -f /dev/shm/*
	./main
	
clean:
	rm -f /dev/shm/*
	rm -f main reader *~ *.o