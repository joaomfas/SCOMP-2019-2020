/*
7. Implement a solution that allows you to share an array of 10 integers between two processes not related hierarchically, a writer and a reader.
	• The writer must create a shared memory area, generate 10 random numbers between 1 and 20 and write them in the shared memory
    [ Note: the writer must be executed before the reader ]
*/

/* Writer Code */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "header.h"

int main() {

    //Cria e abre a zona de memória partilhada
	int fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		perror("failed shm_open!!!\n");
		exit(EXIT_FAILURE);
	}

    //Define o tamanho da zona de memória e inicializa-a com zeros
	if(ftruncate(fd, DATA_SIZE) == -1) {
		perror("failed ftruncate!!!\n");
		exit(EXIT_FAILURE);
	}

    //Mapeia a zona de memória, offset a zero indica que se vai usar toda a memória desde o início
	int* shared_data = (int*) mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(shared_data == MAP_FAILED) {
		perror("failed mmap!!!\n");
		exit(EXIT_FAILURE);
	}

    //Gera 10 números aleatórios(entre '1' e '20') e escreve na memória partilhada
    int i;
    for(i=0; i<DATA_SIZE; i++)
        *(shared_data + i) = (rand() % (20 - 1)) + 1;

    //Fecha mapeamento
    if (munmap(shared_data, DATA_SIZE) == -1) {
		perror("failed munmap!!!\n");
		exit(EXIT_FAILURE);
	}

    //Fecha zona de memória partilhada
	if(close(fd) == -1) {
		perror("failed munmap!!!\n");
		exit(EXIT_FAILURE);
	}

    return 0;
}