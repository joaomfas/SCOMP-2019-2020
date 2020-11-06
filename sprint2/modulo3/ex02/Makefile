# Makefile
# Variables
INCLUDES = header.h
SOURCES = leitor.c escritor.c
OBJFILES = leitor.o escritor.o
EXEC = ex01

all: escritor leitor

# Suffix rules
.SUFFIXES : .c .o
.SUFFIXES : .s .o

# Transform .c into .o
.c.o:
	gcc -Wall -g -c $<

# Transform .s into .o
.s.o:
	gcc -Wall -g -c $<	
	
escritor.o: escritor.c header.h
	gcc -lrt -lm -Wall -g -c escritor.c

escritor: escritor.o
	gcc -lrt -lm -Wall -g -o escritor escritor.o


leitor.o: leitor.c header.h
	gcc -lrt -lm -Wall -g -c leitor.c

leitor: leitor.o
	gcc -lrt -lm -Wall -g -o leitor leitor.o

run_escritor: escritor
	./escritor
	
run_leitor: leitor
	./leitor
	
debug_leitor: leitor
	gdb -tui leitor

debug_escritor: escritor
	gdb -tui escritor

test_leitor: leitor
	valgrind ./leitor

test_escritor: escritor
	valgrind ./escritor

#	valgrind ./${EXEC}
	
clean:
	rm -f escritor leitor *~ *.o

#Git configurations
gitconfig:
	git config --global user.email "1181436@isep.ipp.pt"
	git config --global user.name "Joao Ferreira"
	git config --global push.default simple
	git config --global credential.helper store

git:
	git add -A
	git commit -m "$m"
	git pull
	git push
	
#debug	
debug: ${EXEC}
	ddd ${EXEC} &	
