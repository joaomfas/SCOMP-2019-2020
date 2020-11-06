#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define SHM_NAME "/shm_ex_13"


#define VECTOR_RESULT_SIZE 10
#define NUM_FILHOS 10
#define PATH_DESC_SIZE 30
#define SEARCH_DESC_SIZE 30

typedef struct {
	char filePath[NUM_FILHOS][PATH_DESC_SIZE];
	char keyWord[NUM_FILHOS][SEARCH_DESC_SIZE];
	int	occurrences[NUM_FILHOS][1];
	int flagInicio;
	int seqEscrita;
} ObjBusca;

void iniciaObjBusca(ObjBusca* objBusca);
int findkeyAndCount(char* keyWord, char* filePath);
int countOccurrences(FILE *file, const char *keyWord);

#endif
