#include "header.h"

int spawn_childs(int n){
    pid_t pid;
    int i;
    for (i = 0; i < n; i++)
    {
        pid = fork();
        if (pid < 0)
            return -1;
        else if (pid == 0)
            return i + 1;
    }
    return 0;
}

void fechar_sem_shm(sem_t *sem, int fd, shared_data_type *shared_data){
	//Fechar os semaforos
	if(sem_close(sem) < 0) {
		fprintf(stderr, "sem_close() failed on %s\n%s\n", SEM_NAME, strerror(errno));
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
}

void apagar_sem_shm(){
	// Remover memoria partilhada
	if(shm_unlink(SHM_NAME) == -1){
		perror("failed shm_unlink()\n");
		exit(EXIT_FAILURE);
	}
	// Remover semaforos
	if(sem_unlink(SEM_NAME) < 0){
		fprintf(stderr, "sem_unlink() failed on %s\n%s\n", SEM_NAME, strerror(errno));
	}
}

int main(){
	const int NUM_FILHOS = 3;
	
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
	
	//Inicia numero de bilhetes vendidos a 0
	shared_data->bilhetes_vendidos = 0;
	
	//Inicia numeros dos bilhetes
	int i;
	for(i = 0; i < NUMBER_TICKETS; i++){
		shared_data->bilhetes[i] = 1000+i+1;
	}
	
	//Semaforos
	// Cria o semáforo para acesso à memoria com valor 1
    sem_t *sem;
    if ((sem = sem_open(SEM_NAME, O_CREAT, 0644, 1)) == SEM_FAILED){
        perror("Error in sem_open()");
        exit(EXIT_FAILURE);
    }
    
	int id = spawn_childs(NUM_FILHOS);
	
	if(id > 0){
		cliente(id);
		fechar_sem_shm(sem, fd, shared_data);
		return 0;
	}
	
	//Espera que os filhos terminem
	for (i = 0; i < NUM_FILHOS; i++){
		wait(NULL);
	}
	
	fechar_sem_shm(sem, fd, shared_data);
	apagar_sem_shm();
	
	return 0;
}
