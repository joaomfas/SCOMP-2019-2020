main.o: main.c ex15.h
	gcc -lpthread -lrt -lm -Wall -g -c main.c

main: main.o
	gcc -lpthread -lrt -lm -Wall -g -o main main.o

run: main
	rm -f /dev/shm/*
	./main
	
clean:
	rm -f /dev/shm/*
	rm -f main reader *~ *.o