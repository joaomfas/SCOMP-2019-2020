#include "header.h"

int inserir(){
	printf("---Inserir entrada---\n");
	
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
	
	sem_wait(sem);
	int num_p;
	char nome_p[255];
	char morada_p[255];
	
	//Aguarda que a memoria seja libertada para guardar o registo

	//Verifica se a memoria tem espaço
	int index = shared_data->saved_records;
	if(index == NUMBER_RECORDS){
		printf("Memoria já se encontra cheia\n");
		sem_post(sem);
		return EXIT_FAILURE;
	}
	
	printf("Numero:\n");
	scanf("%d", &num_p);
	printf("Nome:\n");
	scanf("%s", nome_p);
	printf("Morada:\n");
	scanf("%s", morada_p);
	
	//Verifica se o numero ja foi usado
	int i;
	for(i = 0; i < NUMBER_RECORDS; i++){
		if(shared_data->records[i].num == num_p){
			printf("Já existe um registo com o num: %d\n", num_p);
			sem_post(sem);
			return EXIT_FAILURE;
		}	
	}
	
	//Grava os dados
	shared_data->records[index].num = num_p;
	strcpy(shared_data->records[index].nome, nome_p);
	strcpy(shared_data->records[index].morada, morada_p);
	
	//Incrementa o numero de registos existentes
	shared_data->saved_records++;
	
	printf("Registo guardado!\n");
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
