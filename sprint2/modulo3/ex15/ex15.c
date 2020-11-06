#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

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
	const int NUM_CHILDREN = 1;
	const int NUMBER_REPETITIONS = 3;
	const int BUFFER_SIZE = 10;
	
	int i = 0;
	int count = 0;
	int flag_filho;
	int array[BUFFER_SIZE*NUMBER_REPETITIONS];
	
	int fd[2];
	int flag[2];
	enum extremidade {
        LEITURA = 0, ESCRITA = 1
    };
	
	if (pipe(fd) == -1 || pipe(flag) == -1) {
        perror("Pipe failed!");
        exit(EXIT_FAILURE);
    }
	
	int id = cria_filhos(NUM_CHILDREN);
    if (id == -1) {
        perror("Fork failed!");
        exit(EXIT_FAILURE);
    }
	
	if (id == 0) { //Código do pai
        close(fd[LEITURA]); // fecha leitor do pai

		while(count < NUMBER_REPETITIONS){

			for(i = 0; i < BUFFER_SIZE; i++){
				array[i] = (count * BUFFER_SIZE) + i;
				printf("escrevi %d\n", (count * BUFFER_SIZE) + i);
			}
			count++;	
			// escreve numa única operação de escrita os valores guardados no array
			if (write(fd[ESCRITA], array, sizeof(array)) == -1) { // abre o escritor do pai
				perror("Write failed!");
			}
			
			if(read(flag[LEITURA], &flag_filho, sizeof(int)) == -1){ // abre leitor do filho
			perror("Read failed!!");
			exit(EXIT_FAILURE);
			}
			
		}
		close(flag[LEITURA]);
		close(fd[ESCRITA]);
		
	} else { //Código dos filhos
		close(fd[ESCRITA]); // fecha escritor do pai
	
		flag_filho = 1;
		while(count < NUMBER_REPETITIONS){
			if(read(fd[LEITURA], array, sizeof(array)) == -1){ // abre leitor do pai
				perror("Read failed!!");
				exit(EXIT_FAILURE);
			}
			
			for(i = 0; i < BUFFER_SIZE; i++){
				printf("Foi lido o valor %d\n", array[i]);
			}
			count++;
			
			if(write(flag[ESCRITA], &flag_filho, sizeof(int)) == -1){ // abre escritor do filho
			perror("Write failed!");
			}
	
		}
		close(flag[ESCRITA]);
		close(fd[LEITURA]);
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

