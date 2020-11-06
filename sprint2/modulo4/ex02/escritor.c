/**
2. Change the last program in a way that the child process reads from the file “Numbers.txt” by order of its process
number (1 to 8). Use 8 semaphores to synchronize between the child processes.
**/

#include "header.h"

int main(int argc, char *argv[]){

	int processSequenceNumber = 0;
	pid_t generatedPid;

	preencheFicheiroNumbers();

	char sems_names[QTD_SEMS][MAX_TAM_STR];
	
	sem_t* sems[QTD_SEMS];

	for(int i=0; i<QTD_SEMS; i++) {
		snprintf(sems_names[i], MAX_TAM_STR, "%s%d", SEMS_NOME_BASE, i);
		if ((sems[i] = sem_open(sems_names[i], O_CREAT, 0644, 1)) == SEM_FAILED ) {
			fprintf(stderr, "sem_open() failed for %s!!!\n%s\n", sems_names[i], strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

    for (int pos = 0; pos < NUM_FILHOS; pos++){
    	processSequenceNumber = pos;
		if(processSequenceNumber == 0) {
			for(int i = 0; i < NUM_FILHOS; i++) sem_wait(sems[i]);
		}
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
	
		printf("\n**** Filho %d - ESPERANDO ET 1****\n", processSequenceNumber);
		if(processSequenceNumber==NUM_FILHOS-1) sem_post(sems[0]);
		sem_wait(sems[processSequenceNumber]);

		readFile(FILE_NUMBERS_NAME, vetorResultadoProcess, startPosition, endPosition);
		printf("\n**** Filho %d - LENDO ****\n", processSequenceNumber);
		sleep(2);

		if(processSequenceNumber < NUM_FILHOS-1) {
			printf("\n**** Filho %d - LIBERANDO LEITURA ****\n", processSequenceNumber);
			 sem_post(sems[processSequenceNumber+1]);
		}
		
		if(processSequenceNumber!=0)sem_wait(sems[processSequenceNumber]);

		preencheFicheiroOutput(vetorResultadoProcess);
		printf("\n**** Filho %d - ESCREVENDO ****\n", processSequenceNumber);
		sleep(1); //apenas para sincronizar o print

		if(processSequenceNumber < NUM_FILHOS-1) {
			printf("\n**** Filho %d - LIBERANDO ESCRITA ****\n", processSequenceNumber);
			 sem_post(sems[processSequenceNumber+1]);
		}
	} else {

		//Codigo para parent
		int status;
		for(int i=0; i<NUM_FILHOS; i++) {
			wait(&status);
			if(WEXITSTATUS(status) != EXIT_SUCCESS) {
				perror("Filho terminou com erro! ");
				exit(EXIT_FAILURE);
			} else {
				printf("\n\n\t\t\t>>>>	Filho %d finalizado", i);
			}
		}

		int qtdLeituras = QTD_NUMBERS_TO_READ*NUM_FILHOS;
		int startPosition = 0;
		int endPosition = startPosition + QTD_NUMBERS_TO_READ*NUM_FILHOS;
		int vetorResultadoParent[qtdLeituras];

		printf("\nPai lendo ficheiro OUTPUT:\n\n");
		readFile(FILE_OUTPUT_NAME,vetorResultadoParent, startPosition, endPosition);

		printf("\nPai Resultado:\n\n");
		for(int pos=0; pos<qtdLeituras; pos++){
			printf("\t\tPos %d = %d ",pos, vetorResultadoParent[pos]);
			for(int j=0; j < 1000; j++) printf("\r"); //apenas para permitir ver o vetor impresso parcialmente
		}
	}

	for(int i=0; i<QTD_SEMS; i++){
		if(sem_close(sems[i]) < 0) {
			fprintf(stderr, "sem_close() failed on %s\n%s\n", sems_names[i], strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	if(generatedPid>0){
		for(int i=0; i<QTD_SEMS; i++){
			if (sem_unlink(sems_names[i]) < 0) {
				fprintf(stderr, "sem_unlink() failed on %s\n%s\n", sems_names[i], strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
		printf("\nFinal\n");
	}
	
	return 0;
}
