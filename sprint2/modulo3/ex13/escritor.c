/**
Implement a program that allows to optimize the search of words in a set of text files in parallel/concurrent
environment.
The parent process should:
• Create an area of shared memory. The memory area must contain, for each child process, the following
information:
– path to the file;
– word to search;
– an integer to store the number of occurrences.
• Create 10 new processes;
• Fill the shared memory area with the information for each child process;
• Wait until the child processes finish their search;
• Print the number of occurrences determined by each child;
• Eliminate the shared memory area.
Each child process should:
• Open the text file assigned to it by the parent (can/should be different for each child process);
• Determine the number of occurrences of the word to search;
• Write the number of occurrences in their position in the shared memory area.
**/

#include "header.h"

int main(int argc, char *argv[]){

	const int DATA_OBJ_BUSCA_SIZE = sizeof(ObjBusca);
	int processSequenceNumber = 0;

	pid_t generatedPid;
	int fd;

	if ((fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) == -1){
		perror("failed shm_open in parent!!!\n");
		exit(EXIT_FAILURE);
	}

	if (ftruncate(fd, DATA_OBJ_BUSCA_SIZE) == -1){
		perror("failed ftruncate!!!\n");
		exit(EXIT_FAILURE);
	}

	ObjBusca* sharedObjBuscaPai = (ObjBusca *)mmap(NULL, DATA_OBJ_BUSCA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (sharedObjBuscaPai == MAP_FAILED){
		perror("failed mmap!!!\n");
		exit(EXIT_FAILURE);
	}

	iniciaObjBusca(sharedObjBuscaPai);

    for (int pos = 0; pos < NUM_FILHOS; pos++){
        processSequenceNumber = pos;
        generatedPid = fork();
        if (generatedPid == -1)
            return (1);
        if (generatedPid == 0)
            break;
    }

	if (generatedPid == 0){
		int psn = processSequenceNumber;
		int fd;
		//Abre a zona de memória criada (previamente) pelo escritor

		if ((fd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR)) == -1){
			perror("failed shm_open in child!!!\n");
			exit(EXIT_FAILURE);
		}

		ObjBusca* sharedObjBuscaProcesso = (ObjBusca *)mmap(NULL, DATA_OBJ_BUSCA_SIZE,  PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

		if (sharedObjBuscaProcesso == MAP_FAILED){
			perror("failed mmap!!!\n");
			exit(EXIT_FAILURE);
		}
		
		//open file and read 
		while (sharedObjBuscaProcesso->seqEscrita != psn);
	
        int countOcurrences = findkeyAndCount(sharedObjBuscaProcesso->keyWord[psn], sharedObjBuscaProcesso->filePath[psn]);
		sharedObjBuscaProcesso->occurrences[psn][0]= countOcurrences;
	
		if((psn!=NUM_FILHOS-1)) {
			sharedObjBuscaProcesso->seqEscrita++;
		}

		while (sharedObjBuscaProcesso->seqEscrita != psn);
		sharedObjBuscaProcesso->seqEscrita--;

		if (munmap(sharedObjBuscaProcesso, DATA_OBJ_BUSCA_SIZE) == -1){
			perror("failed munmap!!!\n");
			exit(EXIT_FAILURE);
		}

		if(psn==0){
			if (shm_unlink(SHM_NAME) == -1){
				printf("\nPSN: %d - UNLINK!",psn);
				perror("failed shm_unlink!\n");
				exit(EXIT_FAILURE);
			}
		}

		exit(EXIT_SUCCESS);
	}

	sharedObjBuscaPai->flagInicio = 1; //permite que filhos comecem a buscar

   for(int count=0; count<NUM_FILHOS; count++){
		int status = wait(&status);
		/**
		if ((status >> 8) == -1){
			printf("Processo leitor encerrou com erro %d\n", (status >> 8));
		}else{
			printf("Processo leitor encerrado com sucesso.\n");
		}
		**/
   	}

	printf("\n\n===========================================================================\n");
	for(int posBusca = 0; posBusca < NUM_FILHOS; posBusca++){
		char* keyWord = sharedObjBuscaPai->keyWord[posBusca];
		int repeat = sharedObjBuscaPai->occurrences[posBusca][0];
		printf("\n=>BUSCA: %d - Palavra chave: \"%s\" - Repetições: %d",posBusca+1, keyWord, repeat);
	}
	printf("\n\n===========================================================================\n\n");

	if (munmap(sharedObjBuscaPai, DATA_OBJ_BUSCA_SIZE) == -1){
		perror("failed munmap!!!\n");
		exit(EXIT_FAILURE);
	}
	if (close(fd) == -1){
		perror("failed munmap!!!\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}