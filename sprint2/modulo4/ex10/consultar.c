#include "header.h"

int consultar(){
	printf("---Consultar entrada---\n");
	
	//Semaforos
    sem_t *sem;
	// Abre o semaforo criado
    if ((sem = sem_open(SEM_NAME, 0)) == SEM_FAILED){
        perror("Error in sem_open()");
        exit(EXIT_FAILURE);
    }
    
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
	
	//Aguarda que a memoria seja libertada para guardar o registo
	sem_wait(sem);
	
	//Verifica se existem registos guardados
	if(shared_data->saved_records == 0){
		printf("Nao existem registos guardados\n");
		sem_post(sem);
		return EXIT_SUCCESS;
	}
		
	int num_p;
	printf("Numero do registo a consultar:\n");
	scanf("%d", &num_p);
	
	int flag = 0;
	
	int i;
	for(i = 0; i < NUMBER_RECORDS; i++){
		if(shared_data->records[i].num == num_p){
			printf("Registo encontrado\n");
			printf("Numero: %d\n", shared_data->records[i].num);
			printf("Nome: %s\n", shared_data->records[i].nome);
			printf("Nome: %s\n", shared_data->records[i].morada);
			
			flag = 1;
			break;
		}
	}
	
	if(flag == 0){
		printf("Nao foram encontrados registos com o numero %d\n", num_p);
	}
	
	//Liberta o semaforo
	sem_post(sem);
	
	//Fechar a memória partilhada
	if (munmap(shared_data, DATA_SIZE) == -1) {
		perror("failed munmap! ");
		exit(EXIT_FAILURE);
	}
	if(close(fd) == -1) {
		perror("failed munmap! ");
		exit(EXIT_FAILURE);
	}
	//Fechar os semaforos
	if(sem_close(sem) < 0) {
		fprintf(stderr, "sem_close() failed on %s\n%s\n", SEM_NAME, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	return EXIT_SUCCESS;
}
