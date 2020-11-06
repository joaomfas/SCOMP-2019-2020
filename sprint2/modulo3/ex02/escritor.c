#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include "header.h"



int main(int argc, char* argv[]) {
	const int DATA_SIZE = sizeof(Produto);

	int fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		perror("failed shm_open!!!\n");
		exit(EXIT_FAILURE);
	}

	if(ftruncate(fd, DATA_SIZE) == -1) {
		perror("failed ftruncate!!!\n");
		exit(EXIT_FAILURE);
	}

	Produto* shared_data = (Produto*) mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(shared_data == MAP_FAILED) {
		perror("failed mmap!!!\n");
		exit(EXIT_FAILURE);
	}

	//Escrita na zona de memória partilhada
	shared_data->novo = 0; //Começa por assinalar que a informação ainda não pode ser lida

	printf("Insira a descricao do produto: ");
	fgets(shared_data->desc, sizeof(shared_data->desc), stdin);
	int tam = strlen(shared_data->desc);
	if(shared_data->desc[tam-1]=='\n'){
		shared_data->desc[tam-1] = '\0';
	}

	printf("Insira o codigo do produto: ");
	scanf("%d", &shared_data->codigo);
	
	printf("Insira o preco do produto: ");
	scanf("%f", &shared_data->preco);


	shared_data->novo = 1; //assinala que a informação já pode ser lida


	if (munmap(shared_data, DATA_SIZE) == -1) {
		perror("failed munmap!!!\n");
		exit(EXIT_FAILURE);
	}

	if(close(fd) == -1) {
		perror("failed munmap!!!\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}

