# Makefile
# Variables
INCLUDES = header.h
SOURCES = ex14.c
OBJFILES = ex14.o
EXEC = ex14b

# Suffix rules
.SUFFIXES : .c .o
.SUFFIXES : .s .o

# Transform .c into .o
.c.o:
	gcc -lrt -lm -Wall -g -c $<
	
${EXEC}: ${OBJFILES}
	gcc -lrt -lm -Wall -g -o ${EXEC} ${OBJFILES}
	
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
