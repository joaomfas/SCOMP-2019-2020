/**
1. Implement a program that creates 8 new processes.
• Each process reads 200 Numbers from a text file (“Numbers.txt”) and write those numbers to the file “Output.txt”;
• At any time only one child should be able to read/write into the files (one process in each of the files);
• At the end, the father process should print the content of the file “Output.txt”
**/

#include "header.h"

int main(int argc, char *argv[]){

	int processSequenceNumber = 0;
	pid_t generatedPid;

	preencheFicheiroNumbers();

	//Inicia semáforo
	sem_t *sem_01;
	sem_t *sem_02;

	if ((sem_01 = sem_open(SEM_01_NAME, O_CREAT, 0644, 1)) == SEM_FAILED){
        perror("Error in sem_open()");
        exit(EXIT_FAILURE);
    }

	if ((sem_02 = sem_open(SEM_02_NAME, O_CREAT, 0644, 1)) == SEM_FAILED){
        perror("Error in sem_open()");
        exit(EXIT_FAILURE);
    }


    for (int pos = 0; pos < NUM_FILHOS; pos++){
    	processSequenceNumber = pos;
        generatedPid = fork();
        if (generatedPid == -1)
            return (1);
        if (generatedPid == 0)
            break;
    }

	if (generatedPid == 0){
		
		int vetorResultadoProcess[QTD_NUMBERS_TO_READ];
		int startPosition = processSequenceNumber*QTD_NUMBERS_TO_READ;
		int endPosition = startPosition+QTD_NUMBERS_TO_READ; 
		sem_wait(sem_01);
		printf("\n**** Filho %d - LENDO ****\n", processSequenceNumber+1);
		readFile(FILE_NUMBERS_NAME, vetorResultadoProcess, startPosition, endPosition);
		sem_post(sem_01);
		sem_wait(sem_02);
		printf("\n**** Filho %d - ESCREVENDO ****\n", processSequenceNumber+1);
		preencheFicheiroOutput(vetorResultadoProcess);
		
		sem_post(sem_02);

		exit(EXIT_SUCCESS);
	}


		for(int count=0; count<NUM_FILHOS; count++){
			int status = wait(&status);
		}

		if(sem_close(sem_01) < 0) {
			fprintf(stderr, "sem_close() failed on %s\n%s\n", SEM_01_NAME, strerror(errno));
			exit(EXIT_FAILURE);
		}

		if(sem_close(sem_02) < 0) {
			fprintf(stderr, "sem_close() failed on %s\n%s\n", SEM_02_NAME, strerror(errno));
			exit(EXIT_FAILURE);
		}


		int qtdLeituras = QTD_NUMBERS_TO_READ*NUM_FILHOS;
		int startPosition = 0;
		int endPosition = startPosition + QTD_NUMBERS_TO_READ*NUM_FILHOS;
		int vetorResultadoParent[qtdLeituras];

		printf("\nPai lendo ficheiro OUTPUT:\n\n");
		readFile(FILE_OUTPUT_NAME,vetorResultadoParent, startPosition, endPosition);

		printf("\nPai Resultado:\n\n");
		for(int pos=0; pos<qtdLeituras; pos++){
			printf("| Pos %d = %d ",pos, vetorResultadoParent[pos]);
		};

		printf("\nFinal\n");
		return 0;
}
