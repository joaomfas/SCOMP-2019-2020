// 16. Consider a bridge with only one lane that permits traffic in both East-West directions. 
// A car can only enter the bridge if the bridge is not occupied by cars going in the opposite direction. 
// If that is the case, the car must wait until the bridge allows traffic in the other direction. 
// Assume that a car takes 30 seconds to cross the bridge. 
// The bridge’s current allowed direction changes whenever the last car finishes its crossing.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <errno.h>
#include <time.h>
#include "ex16.h"

/**
 * devolve 0 no processo pai, devolve >0 nos processos filho
 */
int cria_filhos(int n) {
	pid_t pid;

	int i;
	for(i=0; i<n; i++) {
		pid=fork();
		if(pid<0)
			return -1;
		else if(pid==0)
			return i+1;
	}
	return 0;
}

int main() {
	
	int i;

	const int REPETICOES = 3;
	const int QTD_SEMS = 3;
	char sems_names[QTD_SEMS][MAX_TAM_STR]; //Nomes para os semáforos, um nome em cada linha da matriz de chars
	int sems_init_values[] = {1, 0, 0}; //Valor inicial para cada um dos semáforos
	enum index{MUTEX, EAST, WEST}; //enumerado para facilitar a leitura do código de acesso aos semáforos

	//Confirma se o número de semáforos está de acordo com o tamanho do vetor de inicializações
	if(sizeof(sems_init_values)/sizeof(sems_init_values[0]) != QTD_SEMS) {
		perror("Incorrect size for sems_init_values!!!\n");
		exit(EXIT_FAILURE);
	}

	/***** Memória Partilhada *****/
	const int DATA_SIZE = sizeof(Bridge);

	//Cria e abre a zona de memória partilhada
	int fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		perror("failed shm_open!!!\n");
		exit(EXIT_FAILURE);
	}

	//Define o tamanho da zona de memória e inicializa-a com zero
	if(ftruncate(fd, DATA_SIZE) == -1) {
		fprintf(stderr, "ftruncate() failed for %s!!!\n%s\n", SHM_NAME, strerror(errno));
		exit(EXIT_FAILURE);
	}

	//Mapeia a zona de memória. Offset a zero indica que se vai usar toda a memória desde o início
	Bridge* bridge = (Bridge*) mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(bridge == MAP_FAILED) {
		fprintf(stderr, "mmap() failed for %s!!!\n%s\n", SHM_NAME, strerror(errno));
		exit(EXIT_FAILURE);
	}

	bridge->east=0;
	bridge->west=0;

	/***** SEMAFOROS *****/
	sem_t* sems[QTD_SEMS];
	for(i=0; i<QTD_SEMS; i++) {
		snprintf(sems_names[i], MAX_TAM_STR, "%s%d", SEM_NAME, i); //gera o nome para o semáforo
		if ((sems[i] = sem_open(sems_names[i], O_CREAT | O_EXCL, 0644, sems_init_values[i])) == SEM_FAILED ) {
			fprintf(stderr, "sem_open() failed for %s!!!\n%s\n", sems_names[i], strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	/***** PROCESSOS *****/
	int id = cria_filhos(NUM_CHILDREN); //Os processos têm de ser criados DEPOIS da memória partilhada e dos semáforos
	if(id == -1) {
		perror("fork() failed!!!\n");
		exit(EXIT_FAILURE);
	}

    sem_wait(sems[MUTEX]);
    if(id%2==0) {
        if(bridge->west==0)
            sem_post(sems[EAST]);
        bridge->east++;
        sem_post(sems[MUTEX]);
            sem_wait(sems[EAST]);
            fprintf(stderr,"\n [%d] a passar para EAST ", id);
            sem_post(sems[EAST]);
            sleep(1);
        sem_wait(sems[MUTEX]);
            bridge->east--;
            if(bridge->east==0)
                sem_post(sems[WEST]);
    } else {
        if(bridge->east==0)
            sem_post(sems[WEST]);
        bridge->west++;
        sem_post(sems[MUTEX]);
            sem_wait(sems[WEST]);
            fprintf(stderr,"\n [%d] a passar para WEST ", id);
            sem_post(sems[WEST]);
            sleep(1);
        sem_wait(sems[MUTEX]);
            bridge->west--;
            if(bridge->west==0)
                sem_post(sems[EAST]);
    }
    sem_post(sems[MUTEX]);

	/***** Código para o pai e filho *****/
	//Fechar os semaforos
	for(i=0; i<QTD_SEMS; i++)
		if(sem_close(sems[i]) < 0) {
			fprintf(stderr, "sem_close() failed on %s\n%s\n", sems_names[i], strerror(errno));
			exit(EXIT_FAILURE);
		}

	//Fechar a memória partilhada
	if (munmap(bridge, DATA_SIZE) == -1) {
		perror("failed munmap!\n");
		exit(EXIT_FAILURE);
	}
	if(close(fd) == -1) {
		perror("failed munmap!\n");
		exit(EXIT_FAILURE);
	}

	/***** Código apenas para o pai *****/
	if(id==0) {
		int status;
		for(i=0; i<NUM_CHILDREN; i++) {
			wait(&status);
			if(WEXITSTATUS(status) != EXIT_SUCCESS) {
				perror("Filho terminou com erro!\n");
				exit(EXIT_FAILURE);
			}
		}
		//Remover a memória partilhada
		if(shm_unlink(SHM_NAME) == -1) {
			perror("failed shm_unlink!\n");
			exit(EXIT_FAILURE);
		}

		//Remover os semaforos
		for(i=0; i<QTD_SEMS; i++)
			if (sem_unlink(sems_names[i]) < 0) {
				fprintf(stderr, "sem_unlink() failed on %s\n%s\n", sems_names[i], strerror(errno));
				exit(EXIT_FAILURE);
			}
	}
	return 0;
}