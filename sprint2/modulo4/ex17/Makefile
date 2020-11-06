# Makefile
# Variables
INCLUDES = header.h
SOURCES = ex17.c
OBJFILES = ex17.o
EXEC = ex17.bin

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
	git config --global user.email "1171865@isep.ipp.pt"
	git config --global user.name "Maria João Dias"
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
