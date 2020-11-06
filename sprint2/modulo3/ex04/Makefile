writer.o: writer.c header.h
	gcc -lrt -lm -Wall -g -c writer.c

writer: writer.o
	gcc -lrt -lm -Wall -g -o writer writer.o


reader.o: reader.c header.h
	gcc -lrt -lm -Wall -g -c reader.c

reader: reader.o
	gcc -lrt -lm -Wall -g -o reader reader.o

run_writer: writer
	rm -f /dev/shm/*
	./writer
	
run_reader: reader
	./reader
	
clean:
	rm -f /dev/shm/*
	rm -f writer reader *~ *.o
