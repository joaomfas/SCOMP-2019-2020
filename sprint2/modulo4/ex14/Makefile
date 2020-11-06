writer.o: writer.c ex14.h
	gcc -lpthread -lrt -lm -Wall -g -c writer.c

writer: writer.o
	gcc -lpthread -lrt -lm -Wall -g -o writer writer.o

reader.o: reader.c ex14.h
	gcc -lpthread -lrt -lm -Wall -g -c reader.c

reader: reader.o
	gcc -lpthread -lrt -lm -Wall -g -o reader reader.o


run: writer
	rm -f /dev/shm/*
	rm -f /dev/sem/*
	./writer

run_writer: writer
	./writer
	./writer

run_reader: reader
	./reader
	./reader
	./reader
	./reader

clean:
	rm -f /dev/shm/*
	rm -f main writer reader *~ *.o