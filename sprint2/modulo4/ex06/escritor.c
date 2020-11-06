/**
6. Change the program above in order to write 15 messages, alternating between father and child.
**/

#include "header.h"

int main(int argc, char *argv[]){

	int processSequenceNumber = 0;
	pid_t generatedPid;

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
		if(processSequenceNumber!=0)sem_wait(sems[processSequenceNumber]);
		printf("\nI'm the child %d\n", processSequenceNumber+1);
	} else {
		//Codigo para parent
		int status;
		for(int i=0; i<NUM_FILHOS; i++) {
			wait(&status);
			if(WEXITSTATUS(status) != EXIT_SUCCESS) {
				perror("Filho terminou com erro! ");
				exit(EXIT_FAILURE);
			} else {
				printf("\nI'm the father %d\n",i+1);
				if(i<NUM_FILHOS-1) sem_post(sems[i+1]);
			}
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
