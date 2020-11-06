# Makefile
# Variables
INCLUDES = 
NAME = ex05
SOURCES = ${NAME}.c
OBJFILES = ${NAME}.o

# Suffix rules
.SUFFIXES : .c .o
.SUFFIXES : .s .o

# Transform .c into .o
.c.o:
	gcc -Wall -g -c $<

# Transform .s into .o
.s.o:
	gcc -Wall -g -c $<

all: ${NAME}	
	
${NAME}: ${OBJFILES}
	gcc -Wall -g -o ${NAME} ./${OBJFILES}

run:	${NAME} 
	./${NAME}

#	valgrind ./${EXEC}
	
clean:
	rm -f ${OBJFILES} ${NAME}

#Git configurations
gitconfig:
	git config --global user.email 1181436@isep.ipp.pt 
	git config --global user.name Joao Ferreira
	git config --global push.default simple

git:
	git add -A
	git commit -m "$m"
	git pull
	git push
	
#debug	
debug: ${EXEC}
	ddd ${EXEC} &	
