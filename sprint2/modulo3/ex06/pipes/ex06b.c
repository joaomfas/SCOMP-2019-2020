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
	
	int num = 0;
	
	int p[2];
	enum extremidade {
        LEITURA = 0, ESCRITA = 1
    };

    if (pipe(p) == -1) {
        perror("Pipe failed!");
        exit(EXIT_FAILURE);
    }
	
	int i;
	const int NUM_CHILDREN = 1;
	int id = cria_filhos(NUM_CHILDREN);
    if (id == -1) {
        perror("Fork failed!");
        exit(EXIT_FAILURE);
    }
    
    if (id == 0) { //Código do pai
        close(p[LEITURA]);
        
        num = rand();
        
        for(i=0; i<ARRAY_SIZE; i++){
			if (write(p[ESCRITA], &num, sizeof (int)) == -1) {
				perror("Write failed!");
			}
		}
        
		close(p[ESCRITA]);

    } else { //Código dos filhos
        
		close(p[ESCRITA]);
        
        int n;
        
        while ((n = read(p[LEITURA], &num, sizeof (int))) > 0);
        
		close(p[LEITURA]);
		
		printf("Valores recebidos!\n");

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
