/**
9. Implement a program to determine the biggest element of an array in a parallel/concurrent environment. The
parent process should:
• Create an array of 1000 integers, initializing it with random values between 0 and 1000;
• Create a shared memory area to store an array of 10 integers, each containing the local maximum of 100 values;
• Create 10 new processes;
• Wait until the 10 child processes finish the search for the local maximum;
• Determine the global maximum;
• Eliminate the shared memory area.
Each child process should:
• Calculate the largest element in the 100 positions;
• Write the value found in the position corresponding to the order number (0-9) in the array of local maximum
**/

#include "header.h"

int main(int argc, char *argv[]){

	const int DATA_VETOR_SIZE = sizeof(VetorEstrutura);
	int processSequenceNumber = 0;
	int iniSubArray;
	int fimSubArray;
	int vetor[VECTOR_SIZE];
	generateVector(vetor);

	pid_t generatedPid;
	int fd;

	if ((fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) == -1){
		perror("failed shm_open in parent!!!\n");
		exit(EXIT_FAILURE);
	}

	if (ftruncate(fd, DATA_VETOR_SIZE) == -1){
		perror("failed ftruncate!!!\n");
		exit(EXIT_FAILURE);
	}

	VetorEstrutura* sharedVetorPai = (VetorEstrutura *)mmap(NULL, DATA_VETOR_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (sharedVetorPai == MAP_FAILED){
		perror("failed mmap!!!\n");
		exit(EXIT_FAILURE);
	}

	iniciaVetorResultado(sharedVetorPai);

    for (int pos = 0; pos < NUM_FILHOS; pos++){
        processSequenceNumber = pos;
		if(pos==0){
			 iniSubArray = 0;
			 fimSubArray = 99;
		} else {
			iniSubArray = pos*100;
			fimSubArray = (pos+1)*100-1;
		}
		
        generatedPid = fork();
        if (generatedPid == -1)
            return (1);
        if (generatedPid == 0)
            break;
    }

	if (generatedPid == 0){
		
		int fd;
		//Abre a zona de memória criada (previamente) pelo escritor
		if ((fd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR)) == -1){
			perror("failed shm_open in child!!!\n");
			exit(EXIT_FAILURE);
		}

		if (ftruncate(fd, DATA_VETOR_SIZE) == -1){
			perror("failed ftruncate!!!\n");
			exit(EXIT_FAILURE);
		}

		VetorEstrutura* sharedVetorProcesso = (VetorEstrutura *)mmap(NULL, DATA_VETOR_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

		if (sharedVetorProcesso == MAP_FAILED){
			perror("failed mmap!!!\n");
			exit(EXIT_FAILURE);
		}
		
		while (sharedVetorProcesso->seqEscrita != processSequenceNumber);
        int foundValue = findMaxValue(vetor, iniSubArray, fimSubArray);
		sharedVetorProcesso->vector[processSequenceNumber] = foundValue;
		if(!(processSequenceNumber==NUM_FILHOS-1)) sharedVetorProcesso->seqEscrita++;


		while (sharedVetorProcesso->seqEscrita != processSequenceNumber);
		sharedVetorProcesso->seqEscrita--;

		if (munmap(sharedVetorProcesso, DATA_VETOR_SIZE) == -1){
			perror("failed munmap!!!\n");
			exit(EXIT_FAILURE);
		}

		if(processSequenceNumber==0){
			if (shm_unlink(SHM_NAME) == -1){
				perror("failed shm_unlink!\n");
				exit(EXIT_FAILURE);
			}
		}

		exit(EXIT_SUCCESS);
	}

	sharedVetorPai->flagInicio = 1; //permite que filhos comecem a buscar

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

	int posVec = 0;
	int foundMaxValue = findMaxValue(sharedVetorPai->vector, posVec, VECTOR_RESULT_SIZE-1);
	printf("\nVetor final:\n");
	for(;posVec < VECTOR_RESULT_SIZE; posVec++){
		printf("\n%d\t%d",posVec+1, sharedVetorPai->vector[posVec]);
	}
	printf("\n\nMaior valor encontrado: %d\n", foundMaxValue);

	if (munmap(sharedVetorPai, DATA_VETOR_SIZE) == -1){
		perror("failed munmap!!!\n");
		exit(EXIT_FAILURE);
	}
	if (close(fd) == -1){
		perror("failed munmap!!!\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}