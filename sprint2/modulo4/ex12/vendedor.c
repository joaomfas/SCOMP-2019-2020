#include "header.h"

int vendedor(int id_cliente){
	printf("Vendedor: A atender o cliente %d\n", id_cliente);
	
    //Memoria partilhada
    //Abre a memoria criada
    //Como já está criada nao usa a oflag O_CREAT
    int fd = shm_open(SHM_NAME, /*O_CREAT |*/ O_RDWR, S_IRUSR | S_IWUSR);
    if(fd == -1){
		perror("failed shm_open()\n");
		exit(EXIT_FAILURE);
	}
	//Define o tamanho e inicializa a 0
    if(ftruncate(fd, DATA_SIZE) == -1){
		perror("failed ftruncate()\n");
	}
    // Mapeia a zona de memoria.
    shared_data_type *shared_data;
    shared_data = (shared_data_type *)mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(shared_data == MAP_FAILED){
		perror("failed mmap()\n");
		exit(EXIT_FAILURE);
	}
	
	//Verifica se existem bilhetes. Caso não existam, retorna -1
	int num_bilhete;
	if(shared_data->bilhetes_vendidos == NUMBER_TICKETS){
		num_bilhete = -1;
		printf("Vendedor: Já não há bilhetes!\n");
	}else{
		num_bilhete = shared_data->bilhetes[shared_data->bilhetes_vendidos];
		printf("Vendedor: Vendi o bilhete %d\n", num_bilhete);
		shared_data->bilhetes_vendidos++;	
	}

	//Fechar a memória partilhada
	if (munmap(shared_data, DATA_SIZE) == -1) {
		perror("failed munmap! ");
		exit(EXIT_FAILURE);
	}
	if(close(fd) == -1) {
		perror("failed munmap! ");
		exit(EXIT_FAILURE);
	}
	
	return num_bilhete;
}
