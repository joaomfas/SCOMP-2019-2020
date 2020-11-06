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
#include "header.h"

int cria_filhos(int n) {
    pid_t pid;

    int i;
    for (i = 0; i < n; i++) {
        pid = fork();
        if (pid < 0) {
            return -1;
        } else if (pid == 0) {
            return i + 1;
        }
    }
    return 0;
}

int main(void) {

	clock_t t;
	t = clock();
	
	int i;
	const int DATA_SIZE = sizeof(S1);
	const int NUM_CHILDREN = 1;
	int id = cria_filhos(NUM_CHILDREN);
    if (id == -1) {
        perror("Fork failed!");
        exit(EXIT_FAILURE);
    }

    if (id == 0) { //Código do pai

		int fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
		if(fd == -1) {
			perror("failed shm_open!!!\n");
			exit(EXIT_FAILURE);
		}

		if(ftruncate(fd, DATA_SIZE) == -1) {
			perror("failed ftruncate!!!\n");
			exit(EXIT_FAILURE);
		}

		S1* shared_data = (S1*) mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		
		if(shared_data == MAP_FAILED) {
			perror("failed mmap!!!\n");
			exit(EXIT_FAILURE);
		}

		shared_data->novo = 0;
		
		for(i=0; i<ARRAY_SIZE; i++){
			shared_data->s2[i].number = i;
			strcpy(shared_data->s2[i].strg, "ISEP –SCOMP 2020");
		}

        shared_data->novo = 1;

        if (munmap(shared_data, DATA_SIZE) == -1) {
			perror("failed munmap!!!\n");
			exit(EXIT_FAILURE);
		}

		if(close(fd) == -1) {
			perror("failed munmap!!!\n");
			exit(EXIT_FAILURE);
		}

    } else { //Código dos filhos
        int fd = shm_open(SHM_NAME, O_RDONLY, 0);
		if(fd == -1) {
			perror("failed shm_open!!!\n");
			exit(EXIT_FAILURE);
		}

		S1* shared_data = (S1*) mmap(NULL, DATA_SIZE, PROT_READ, MAP_SHARED, fd, 0);
		if(shared_data == MAP_FAILED) {
			perror("failed mmap!!!\n");
			exit(EXIT_FAILURE);
		}

		while(shared_data->novo != 1);

		printf("Valores recebidos!\n");

		if (munmap(shared_data, DATA_SIZE) == -1) {
			perror("failed munmap!\n");
			exit(EXIT_FAILURE);
		}

		if(shm_unlink(SHM_NAME) == -1) {
			perror("failed shm_unlink!\n");
			exit(EXIT_FAILURE);
		}

        exit(EXIT_SUCCESS);
    }

    //Pai espera que os filhos terminem
    int status;
    for (i = 0; i < NUM_CHILDREN; i++) {
        pid_t p = wait(&status);
        if (p == -1) {
            perror("Wait failed!");
            exit(EXIT_FAILURE);
        }
    }

	t = clock() - t;
	double time = ((double)t)/CLOCKS_PER_SEC;
	printf("O processo com shared memory levou %f segundos\nNúmero de elementos: %d\n", time, ARRAY_SIZE);

    return 0;
}
