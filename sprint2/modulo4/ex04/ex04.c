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

int main(){

    shared_data_type *shared_data;
    const int DATA_SIZE = sizeof(shared_data_type);
    const int NUM_CHILDS = 11;
    const int LIMIT_TIMEOUT = 12;

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
    shared_data = (shared_data_type *)mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(shared_data == MAP_FAILED){
		perror("failed mmap()\n");
		exit(EXIT_FAILURE);
	}

    shared_data->written = 0;
	
	//Semaforos
    sem_t *sem;
	// Inicia o semáforo com o valor 1
    if ((sem = sem_open(SEM_NAME, O_CREAT, 0644, 1)) == SEM_FAILED){
        perror("Error in sem_open()");
        exit(EXIT_FAILURE);
    }

	//Processos
    int id = spawn_childs(NUM_CHILDS);

    if (id > 0){ //Código do filho
		
		// Struct para intervalo de tempo
		struct timespec ts;
		if(clock_gettime(CLOCK_REALTIME, &ts) == -1){
			perror("clock_gettime failed\n");
			exit(EXIT_FAILURE);
		}
		ts.tv_sec += LIMIT_TIMEOUT; // Definir o limite para time out
		
		int t;
		t = sem_timedwait(sem, &ts); // Aguarda o semaforo. s = -1 caso existe erro ou time out
		
		if(t == -1){
			if(errno == ETIMEDOUT){
				printf("sem_timedwait() time out\n");
				exit(EXIT_FAILURE);
			}else{
				perror("sem_timedwait() failed\n");
				exit(EXIT_FAILURE);
			}
		}
		
		if(NUM_STRS == shared_data -> written){
			printf("Memoria cheia, pretende eliminar um registo? Y/N\n");
			char answer[2];
			fgets(answer, 2, stdin);
			answer[1] = '\0';
			if(strcmp(answer, "Y") == 0 || strcmp(answer, "y") == 0){
				int index = 0;
                printf("Qual o registo a eliminar? 0-%d\n", NUM_STRS-1);
                scanf("%d", &index);

                if (index >= NUM_STRS || index < 0){
                    perror("Valor inválido!");
                    exit(EXIT_FAILURE);
                }
                else{
                    sprintf(shared_data->str[index], "i am process with id %d", id);
                    sleep(1);
                }
			}else{
				printf("Processo %d nao escreveu na memoria\n", id);
			}
			sem_post(sem);
		}else{
			printf("vou escrever - id %d\n", id);
			sprintf(shared_data->str[shared_data->written], "i am process with id %d", id);
			shared_data->written++;
			sleep(1);
			sem_post(sem); // Liberta a memória para outro processo usar
		}	
    }else{ // Código do pai		
		int i;
		// Espera que os filhos terminem
		for (i = 0; i < NUM_CHILDS; i++){
			wait(NULL);
		}
    
		for (i = 0; i < NUM_CHILDS; i++){
			printf("%s\n", shared_data->str[i]);
		}
	}

	// Código para pai e filhos
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

	if(id == 0){ // Código apenas para o pai
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

    return 0;
}
