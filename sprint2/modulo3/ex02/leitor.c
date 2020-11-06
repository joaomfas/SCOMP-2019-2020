#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "header.h"


int main(int argc, char* argv[]) {
	const int DATA_SIZE = sizeof(Produto);

	//Abre a zona de memória criada (previamente) pelo escritor
	int fd = shm_open(SHM_NAME, O_RDONLY, 0);
	if(fd == -1) {
		perror("failed shm_open!!!\n");
		exit(EXIT_FAILURE);
	}

	//Atendendo a que estamos apenas a utilizar a memória partilhada em modo de leitura não se pode utilizar o ftruncate
/*
	if(ftruncate(fd, DATA_SIZE) == -1) {
		perror("failed ftruncate!!!\n");
		exit(EXIT_FAILURE);
	}
*/

	Produto* shared_data = (Produto*) mmap(NULL, DATA_SIZE, PROT_READ, MAP_SHARED, fd, 0);
	if(shared_data == MAP_FAILED) {
		perror("failed mmap!!!\n");
		exit(EXIT_FAILURE);
	}

	while(shared_data->novo != 1); //espera ativa para que existam temperaturas para ler

	printf("Código do produto: %d\n", shared_data->codigo);
	printf("Descricao do produto: %s\n", shared_data->desc);
	printf("Preco do produto: %f\n", shared_data->preco);


	if (munmap(shared_data, DATA_SIZE) == -1) {
		perror("failed munmap!\n");
		exit(EXIT_FAILURE);
	}

	if(shm_unlink(SHM_NAME) == -1) {
		perror("failed shm_unlink!\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
