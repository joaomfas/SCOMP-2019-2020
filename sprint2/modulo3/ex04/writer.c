/*
4.Implement a solution that allows you to share an array of 100 characters 
    between two processes not related hierarchically, a writer and a reader.
        •The writer must create a shared memory area, generate 100 random 
            chars(between ‘A’ and ‘Z’) and write them in the shared memory.
        •The reader should read the 100 values, calculate the sum of ASCII 
            code and print all the chars and the average of the 100 ASCII codes.

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
	char* shared_data = (char*) mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(shared_data == MAP_FAILED) {
		perror("failed mmap!!!\n");
		exit(EXIT_FAILURE);
	}

    //Gera 100 números aleatórios(entre 'A' e 'Z') e escreve na memória partilhada
    int i;
    for(i=0; i<DATA_SIZE; i++)
        *(shared_data + i) = (rand() % ('Z' - 'A' + 1)) + 'A';

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