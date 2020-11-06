#include "header.h"

int cliente(int id){
	
	const int MIN_RAND = 1;
	const int MAX_RAND = 10;
	srand((unsigned)time(NULL));
	
	//Semaforos
    sem_t *sem;
	// Abre o semaforo criado
    if ((sem = sem_open(SEM_NAME, 0)) == SEM_FAILED){
        perror("Error in sem_open()");
        exit(EXIT_FAILURE);
    }
    
	//Aguarda que a memoria seja libertada para adquirir bilhete
	printf("Cliente %d: À espera da minha vez\n", id);
	sem_wait(sem);
	
	//Cliente demora entre 1-10s a ser atendido
	sleep(rand()%(MAX_RAND-MIN_RAND+1)+MIN_RAND);
	int num_bilhete = vendedor(id);
	if(num_bilhete == -1){
		printf("Cliente %d: Bilhetes esgotados!\n", id);
	}else{
		printf("Cliente %d: Adquiri o bilhete %d\n", id, num_bilhete);
	}
	
	//Liberta o semaforo
	sem_post(sem);

	//Fechar os semaforos
	if(sem_close(sem) < 0) {
		fprintf(stderr, "sem_close() failed on %s\n%s\n", SEM_NAME, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	return EXIT_SUCCESS;
}
