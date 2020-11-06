/**
1. Implement a solution that sends the number and name of a student between two processes not related
hierarchically, a writer and a reader.
• The writer must create a shared memory area, read the data from the keyboard and write them in the shared
memory.
• The reader should read the data from the shared memory and print them on the screen.
Note: the writer must be executed before the reader.
**/

#include "header.h"


int main(int argc, char* argv[]) {
	
	int fd;
	if((fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
		perror("failed shm_open!!!\n");
		exit(EXIT_FAILURE);
	}

	if(ftruncate(fd, DATA_ALUNO_SIZE) == -1) {
		perror("failed ftruncate!!!\n");
		exit(EXIT_FAILURE);
	}

	Aluno* sharedDataAlunoEscritor = (Aluno*) mmap(NULL, DATA_ALUNO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if(sharedDataAlunoEscritor == MAP_FAILED) {
		perror("failed mmap!!!\n");
		exit(EXIT_FAILURE);
	}
	
	sharedDataAlunoEscritor->flagLeitura = 0;

	//Solicita dados
	printf("\nEntre com a matricula do aluno: ");
	scanf("%d", &sharedDataAlunoEscritor->matricula);
	getc(stdin);
	fflush(stdin);
	printf("\nEntre com o nome do aluno: ");
	fgets(sharedDataAlunoEscritor->nomeAluno, sizeof(sharedDataAlunoEscritor->nomeAluno), stdin);

	//verifica a existência de '\n' e substitui por \0
	int tam = strlen(sharedDataAlunoEscritor->nomeAluno);
	if(sharedDataAlunoEscritor->nomeAluno[tam-1]=='\n') sharedDataAlunoEscritor->nomeAluno[tam-1] = '\0';

	sharedDataAlunoEscritor->flagLeitura = 1; //muda a flag sinalizando que a leitura pode ser realizada

	if (munmap(sharedDataAlunoEscritor, DATA_ALUNO_SIZE) == -1) {
		perror("failed munmap!!!\n");
		exit(EXIT_FAILURE);
	}

	if(close(fd) == -1) {
		perror("failed munmap!!!\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}

