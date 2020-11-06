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

#define SHM_NAME "/shm_ex_5"
#define REPETICOES 1000

typedef struct {
	int valueA;
 	int valueB;
	int repeticoes;
	int flagLeitura;
} Valores;

void defineValoresObj(Valores* valores){
	valores->valueA = 8000;
	valores->valueB = 200;
	valores->flagLeitura = 0;
	valores->repeticoes = REPETICOES;
}

const int DATA_VALORES_SIZE = sizeof(Valores);

#endif
