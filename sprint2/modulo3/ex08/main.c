// 8. Implement a program that creates a shared memory area to store an integer, 
// initializes this value to 100, and creates a new process. 
// Parent and child must perform the followingo perations 1.000.000 times:
//  • Increase the value;
//  • Decrease the value;
//  • Only write the value on the screen at the end.
// Review the results. Will these results always be correct?

// RESPOSTA : Não, a escrita no fich pode ocorrer a meio de outra escrita 
// e gerar resultados imprevisíveis..

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

int main() {
    const int DATA_SIZE = sizeof(StoredInt);
    
    int i;
    
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

		StoredInt* shared_data = (StoredInt*) mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if(shared_data == MAP_FAILED) {
			perror("failed mmap!!!\n");
			exit(EXIT_FAILURE);
		}

        shared_data->Integer = INIT_VALUE;

        shared_data->novo = 1;

        for(i=0; i<OPERATIONS; i++) {
            shared_data->Integer+=1;
            shared_data->Integer-=1;
        }

        printf("(pai 1) SHARED INT -> %d\n", shared_data->Integer);

        if (munmap(shared_data, DATA_SIZE) == -1) {
			perror("failed munmap!!!\n");
			exit(EXIT_FAILURE);
		}
		if(close(fd) == -1) {
			perror("failed munmap!!!\n");
			exit(EXIT_FAILURE);
		}

    } else { //Código dos filhos
 

        int fd = shm_open(SHM_NAME, O_RDWR, 0);
		if(fd == -1) {
			perror("failed shm_open!!!\n");
			exit(EXIT_FAILURE);
		}
		
		StoredInt* shared_data = (StoredInt*) mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if(shared_data == MAP_FAILED) {
			perror("failed mmap!!!\n");
			exit(EXIT_FAILURE);
		}

		while(shared_data->novo != 1);
        for(i=0; i<OPERATIONS; i++) {
            shared_data->Integer+=1;
            shared_data->Integer-=1;
        }
        
		printf("(filho) SHARED INT -> %d\n", shared_data->Integer);
		
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

    return 0;
}