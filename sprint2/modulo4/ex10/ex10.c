#include "header.h"

int menu(){
	int op;
	
	printf("---MENU---\n");
	printf("1. Inserir\n2. Consultar info\n3. Consultar toda a info\n0. Sair\nOpcao: ");
	scanf("%d", &op);
	
    return op;
}

void apagar_shm_sem(){	
	// Remover memoria partilhada
	if(shm_unlink(SHM_NAME) == -1){
		perror("failed shm_unlink()\n");
		exit(EXIT_FAILURE);
	}
	// Remover semaforos
	if(sem_unlink(SEM_NAME) < 0){
		fprintf(stderr, "sem_unlink() failed on %s\n%s\n", SEM_NAME, strerror(errno));
	}
	if(sem_unlink(SEM_APP_NAME) < 0){
		fprintf(stderr, "sem_unlink() failed on %s\n%s\n", SEM_APP_NAME, strerror(errno));
	}
	
	printf("SHM e SEM limpos!\n");
}

int main(){
	// Memoria partilhada
    // Cria e abre a zona de memoria partilhada
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
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
	
	//Semaforos
	// Cria o semáforo para acesso à memoria com valor 1
    sem_t *sem;
    if ((sem = sem_open(SEM_NAME, O_CREAT, 0644, 1)) == SEM_FAILED){
        perror("Error in sem_open()");
        exit(EXIT_FAILURE);
    }
    //Cria semaforo que controla o numero de instancias abertas
	sem_t *sem_app;
	if ((sem_app = sem_open(SEM_APP_NAME, O_CREAT, 0644, 0)) == SEM_FAILED){
        perror("Error in sem_open()");
        exit(EXIT_FAILURE);
    }

	sem_post(sem_app);
	
	int op;
	
	do{
		op = menu();
		switch(op){
			case 1:
				inserir();
				break;
			case 2:
				consultar();
				break;
			case 3:
				consultar_todos();
				break;
			case 0:
				break;
			default:
				printf("Opçao invalida\n");
				op = menu();
		}
	}while(op != 0);
	
	sem_wait(sem_app);
	
	//Determina se existem mais instancias abertas
	int instancias = 0;
	if(sem_getvalue(sem_app, &instancias) == -1){
		perror("sem_getvalue() failed");
		exit(EXIT_FAILURE);
	}
	
	//Fechar os semaforos
	if(sem_close(sem) < 0) {
		fprintf(stderr, "sem_close() failed on %s\n%s\n", SEM_NAME, strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(sem_close(sem_app) < 0) {
		fprintf(stderr, "sem_close() failed on %s\n%s\n", SEM_APP_NAME, strerror(errno));
		exit(EXIT_FAILURE);
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
	
	//Caso nao existam mais instancias abertas, elimina a memoria e o semaforo
	if(instancias == 0){
		apagar_shm_sem();
	}
	
	return 0;
}
