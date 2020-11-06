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

#define SHM_NAME "/shm_ex_9"


#define RANGE_NUM_AVALIADOS 100
#define MAX_NUM 1000
#define MIN_NUM 0
#define VECTOR_RESULT_SIZE 10
#define NUM_FILHOS VECTOR_RESULT_SIZE
#define VECTOR_SIZE RANGE_NUM_AVALIADOS*VECTOR_RESULT_SIZE

typedef struct {
	int vector[VECTOR_RESULT_SIZE];
	int seqEscrita;
	int flagInicio;
} VetorEstrutura;

void iniciaVetorResultado(VetorEstrutura* estrutura);
void generateVector(int[]);
int findMaxValue(int [], int inicio, int fim);
int randomNumber(int x);
void milliSleep(int time);
char* defineLoadPos(int count);

#endif
