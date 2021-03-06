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
	
	const int DATA_SIZE = sizeof(S1);
	int fd[2];
	enum extremidade {
        LEITURA = 0, ESCRITA = 1
    };

    if (pipe(fd) == -1) {
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
    
	S1* pipe_data;
	pipe_data = (S1*)malloc(ARRAY_SIZE * DATA_SIZE);
		
    if (id == 0) { //Código do pai
        close(fd[LEITURA]);

		// guarda os 2 valores num array de estrutura S1	
		for(i=0; i<ARRAY_SIZE; i++){
			pipe_data->s2[i].number = i;
			strcpy(pipe_data->s2[i].strg, "ISEP –SCOMP 2020");
		}
		
		// escreve numa única operação de escrita os valores guardados na estrutura
 		if (write(fd[ESCRITA], pipe_data, ARRAY_SIZE * DATA_SIZE) == -1) {
			perror("Write failed!");
		}
        
		close(fd[ESCRITA]);

    } else { //Código dos filhos
        
		close(fd[ESCRITA]);
        
		S1* pipe_data_child;
		pipe_data_child = (S1*)malloc(ARRAY_SIZE * DATA_SIZE);
        while (read(fd[LEITURA], pipe_data_child, ARRAY_SIZE * DATA_SIZE) > 0);
        
		close(fd[LEITURA]);
		
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
	printf("O processo com pipe levou %f segundos\nNúmero de elementos: %d\n", time, ARRAY_SIZE);
	
    return 0;
}
