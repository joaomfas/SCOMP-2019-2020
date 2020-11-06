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

#define SHM_NAME "/shm_ex_1"
#define TAMANHO_NOME 30

typedef struct {
	int matricula;
	char nomeAluno[TAMANHO_NOME];
	int flagLeitura;
} Aluno;

const int DATA_ALUNO_SIZE = sizeof(Aluno);

#endif
