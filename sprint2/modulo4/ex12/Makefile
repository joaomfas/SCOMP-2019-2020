# Makefile
# Variables
INCLUDES = header.h
SOURCES = ex12.c cliente.c vendedor.c
OBJFILES = ex12.o cliente.o vendedor.o
EXEC = ex12.bin

# Suffix rules
.SUFFIXES : .c .o
.SUFFIXES : .s .o

# Transform .c into .o
.c.o:
	gcc -lrt -lm -Wall -g -c $<
	
${EXEC}: ${OBJFILES}
	gcc -lrt -lm -Wall -g -o ${EXEC} ${OBJFILES} -lpthread -lrt
	
${OBJFILES}: ${SOURCES} ${INCLUDES}

run: ${EXEC}
	./${EXEC}

#	valgrind ./${EXEC}
	
clean:
	rm -f /dev/shm/*
	rm -f /dev/sem/*
	rm -f ${OBJFILES} ${EXEC}

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
