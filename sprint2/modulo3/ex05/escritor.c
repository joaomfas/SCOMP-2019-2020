/**
5. Implement a program that creates a shared memory area to store two integers and initializes those integers with
the values 8000 and 200 and creates a new process. Parent and child must perform the following operations
1.000.000 times:
• The father will increase the first value and decrease the second value.
• The child will decrease the first value and increase the second value.
 Only write the value on the screen at the end. Review the results. Will these results always be correct?
**/

#include "header.h"

int main(int argc, char *argv[]){

	pid_t generatedPid;
	int fd;

	if ((fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) == -1){
		perror("failed shm_open!!!\n");
		exit(EXIT_FAILURE);
	}

	if (ftruncate(fd, DATA_VALORES_SIZE) == -1){
		perror("failed ftruncate!!!\n");
		exit(EXIT_FAILURE);
	}

	Valores* sharedDataValoresEscritor = (Valores*)mmap(NULL, DATA_VALORES_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (sharedDataValoresEscritor == MAP_FAILED){
		perror("failed mmap!!!\n");
		exit(EXIT_FAILURE);
	}

	defineValoresObj(sharedDataValoresEscritor);
	//sharedDataValoresEscritor->flagLeitura = 0;
	

	generatedPid = fork();
	if (generatedPid == -1) return (1);

	if (generatedPid == 0){
		int fd;

		//Abre a zona de memória criada (previamente) pelo escritor
		if ((fd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR)) == -1){
			perror("failed shm_open!!!\n");
			exit(EXIT_FAILURE);
		}

		if (ftruncate(fd, DATA_VALORES_SIZE) == -1)	{
			perror("failed ftruncate!!!\n");
			exit(EXIT_FAILURE);
		}

		Valores* sharedDataValoresLeitor = (Valores *)mmap(NULL, DATA_VALORES_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

		if (sharedDataValoresLeitor == MAP_FAILED){
			perror("failed mmap!!!\n");
			exit(EXIT_FAILURE);
		}

		while(sharedDataValoresLeitor->repeticoes >= 0){
			while (sharedDataValoresLeitor->flagLeitura != 1); 
			//The child will decrease the first value and increase the second value.
			sharedDataValoresLeitor->valueA--;
			sharedDataValoresLeitor->valueB++;
			sharedDataValoresLeitor->flagLeitura = 0;
		}

		if (munmap(sharedDataValoresLeitor, DATA_VALORES_SIZE) == -1){
			perror("failed munmap!\n");
			exit(EXIT_FAILURE);
		}

		if (shm_unlink(SHM_NAME) == -1){
			perror("failed shm_unlink!\n");
			exit(EXIT_FAILURE);
		}

		exit(EXIT_SUCCESS);
	}

	while(sharedDataValoresEscritor->repeticoes>=0){
		while (sharedDataValoresEscritor->flagLeitura != 0);
		//The father will increase the first value and decrease the second value.
		sharedDataValoresEscritor->valueA++;
		sharedDataValoresEscritor->valueB--;
		printf("\rRepeticao: %d", sharedDataValoresEscritor->repeticoes);
		
		sharedDataValoresEscritor->repeticoes--;
		sharedDataValoresEscritor->flagLeitura = 1;

	}

	int status = wait(&status);
	printf("\n=========================================================================\n");
	if ((status >> 8) == -1){
		printf("Processo leitor encerrou com erro %d\n", (status >> 8));
	}
	else{
		printf("Processo leitor encerrado com sucesso.\n");
		printf("Valor A %d \nValor B %d", sharedDataValoresEscritor->valueA,  sharedDataValoresEscritor->valueB );
	    printf("\n=========================================================================\n");
	}

	if (munmap(sharedDataValoresEscritor, DATA_VALORES_SIZE) == -1){
		perror("failed munmap!!!\n");
		exit(EXIT_FAILURE);
	}
	if (close(fd) == -1){
		perror("failed munmap!!!\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}