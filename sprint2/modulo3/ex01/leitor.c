/**
1. Implement a solution that sends the number and name of a student between two processes not related
hierarchically, a writer and a reader.
• The writer must create a shared memory area, read the data from the keyboard and write them in the shared
memory.
• The reader should read the data from the shared memory and print them on the screen.
Note: the writer must be executed before the reader.
**/

#include "header.h"

int main(int argc, char* argv[]) {

	//Abre a zona de memória criada (previamente) pelo escritor
	int fd;
	if((fd = shm_open(SHM_NAME, O_RDONLY, 0)) == -1) {
		perror("failed shm_open!!!\n");
		exit(EXIT_FAILURE);
	}

	Aluno* sharedDataAlunoLeitor = (Aluno*) mmap(NULL, DATA_ALUNO_SIZE, PROT_READ, MAP_SHARED, fd, 0);

	if(sharedDataAlunoLeitor == MAP_FAILED) {
		perror("failed mmap!!!\n");
		exit(EXIT_FAILURE);
	}

	while(sharedDataAlunoLeitor->flagLeitura != 1); //espera ativa para que existam temperaturas para ler

	printf("\nMatricula do aluno: %d\n", sharedDataAlunoLeitor->matricula);
	printf("Nome do aluno: %s\n", sharedDataAlunoLeitor->nomeAluno);


	if (munmap(sharedDataAlunoLeitor, DATA_ALUNO_SIZE) == -1) {
		perror("failed munmap!\n");
		exit(EXIT_FAILURE);
	}

	if(shm_unlink(SHM_NAME) == -1) {
		perror("failed shm_unlink!\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
