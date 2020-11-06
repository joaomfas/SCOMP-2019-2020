main.o: main.c ex13.h
	gcc -lpthread -lrt -lm -Wall -g -c main.c

main: main.o
	gcc -lpthread -lrt -lm -Wall -g -o main main.o

run: main
	rm -f /dev/shm/*
	rm -f /dev/sem/*
	./main
	
clean:
	rm -f /dev/shm/*
	rm -f main reader *~ *.o