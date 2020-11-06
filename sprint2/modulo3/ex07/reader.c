/*
7. Implement a solution that allows you to share an array of 10 integers between two processes not related hierarchically, a writer and a reader.
	• The reader should read the 10 values, calculate and print the average.
    [ Note: the writer must be executed before the reader ]
*/

/* Reader Code */

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

    //Abre a zona de memória criada (previamente) pelo escritor
	int fd = shm_open(SHM_NAME, O_RDONLY, 0);
	if(fd == -1) {
		perror("failed shm_open!!!\n");
		exit(EXIT_FAILURE);
	}

    //Mapeia a zona de memória, offset a zero indica que se vai usar toda a memória desde o início
    int* shared_data = (int*) mmap(NULL, DATA_SIZE, PROT_READ, MAP_SHARED, fd, 0);
	if(shared_data == MAP_FAILED) {
		perror("failed mmap!!!\n");
		exit(EXIT_FAILURE);
	}

    //Lê os 10 inteiros, soma os valores e apresenta a média.
    int i;
    int avg=0;
    for(i=0; i<DATA_SIZE; i++) {
        printf("%d ", *(shared_data + i));
        avg+=*(shared_data + i);
    }
    avg/=DATA_SIZE;
    printf("\nA média dos %d números é: %d \n", DATA_SIZE, avg);

    //Fecha mapeamento
    if (munmap(shared_data, DATA_SIZE) == -1) {
		perror("failed munmap!\n");
		exit(EXIT_FAILURE);
	}

    //Apaga memória partilhada
	if(shm_unlink(SHM_NAME) == -1) {
		perror("failed shm_unlink!\n");
		exit(EXIT_FAILURE);
	}

    return 0;
}