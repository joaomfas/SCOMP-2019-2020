# Makefile
# Variables
INCLUDES = header.h
SOURCES = leitor.c escritor.c
OBJFILES = leitor.o escritor.o
EXEC = ex01

all: escritor
	./escritor

# Suffix rules
.SUFFIXES : .c .o
.SUFFIXES : .s .o

# Transform .c into .o
.c.o:
	gcc -Wall -g -c $<

# Transform .s into .o
.s.o:
	gcc -Wall -g -c $<	
	
escritor.o: escritor.c 
	gcc -lrt -lm -Wall -g -c escritor.c 

escritor: escritor.o
	gcc -lrt -lm -Wall -g -o escritor escritor.o funcoes.c header.h

run_escritor: escritor
	./escritor

debug_escritor: escritor
	gdb -tui escritor

test_escritor: escritor
	valgrind ./escritor

#	valgrind ./${EXEC}
	
clean:
	rm -f *.o
	rm -f *.exe
	rm arqs/*
	rm -f *.exe.stackdump
	rm /dev/shm/* 

cleanShm:
	rm /dev/shm/* 