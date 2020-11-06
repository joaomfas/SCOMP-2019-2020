# Makefile
# Variables
INCLUDES = 
SOURCES = ex13.c
OBJFILES = ex13.o
EXEC = ex13

all: ex13

# Suffix rules
.SUFFIXES : .c .o
.SUFFIXES : .s .o

# Transform .c into .o
.c.o:
	gcc -Wall -g -c $<

# Transform .s into .o
.s.o:
	gcc -Wall -g -c $<	
	
${EXEC}: ${OBJFILES}
	gcc -Wall -g -o ${EXEC} ${OBJFILES}
	
${OBJFILES}: ${SOURCES} ${INCLUDES}

run: ${EXEC}
	./${EXEC}

#	valgrind ./${EXEC}
	
clean:
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
