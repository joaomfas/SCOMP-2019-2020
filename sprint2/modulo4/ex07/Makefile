# Makefile
# Variables
SOURCES = ex07.c
OBJFILES = ex07.o
EXEC = ex07

# Suffix rules
.SUFFIXES : .c .o
.SUFFIXES : .s .o

# Transform .c into .o
.c.o:
	gcc -lpthread -lrt -lm -Wall -g -c $<
	
${EXEC}: ${OBJFILES}
	gcc -lpthread -lrt -lm -Wall -g -o ${EXEC} ${OBJFILES}
	
${OBJFILES}: ${SOURCES} ${INCLUDES}

run: ${EXEC}
	./${EXEC}

#	valgrind ./${EXEC}
	
clean:
	rm -f ${OBJFILES} ${EXEC}

#Git configurations
gitconfig:
	git config --global user.email "1171865@isep.ipp.pt"
	git config --global user.name "Maria Joao Dias"
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
