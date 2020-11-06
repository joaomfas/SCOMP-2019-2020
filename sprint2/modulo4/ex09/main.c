// 9.Consider the two following processes:
// ======================================== //
//     Process 1      |     Process 2       //
// ---------------------------------------- //
//    buy_chips();    |  eat_and_drink();   //
// ======================================== //
// Add semaphores such that will ensure that neither of P1 or P2 
// eat and drink until  both the beer and chips are acquired!

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
#include "ex09.h"

int cria_filhos(int n) {
    pid_t pid;
    int i;
    for (i = 0; i < n; i++) {
        pid = fork();
        if (pid < 0) {
            return -1;
        } else if (pid == 0) {
            return i + 1;
        }
    }
    return 0;
}

int main() {
    int i, j;

	const int REPETICOES = 3;
	const int QTD_SEMS = 2;
	char sems_names[QTD_SEMS][MAX_TAM_STR]; //Nomes para os semáforos, um nome em cada linha da matriz de chars
	int sems_init_values[] = {1, 0}; //Valor inicial para cada um dos semáforos
	
	//Confirma se o número de semáforos está de acordo com o tamanho do vetor de inicializações
	if(sizeof(sems_init_values)/sizeof(sems_init_values[0]) != QTD_SEMS) {
		perror("Incorrect size for sems_init_values!!!\n");
		exit(EXIT_FAILURE);
	}

	/***** Memória Partilhada *****/
	const int DATA_SIZE = sizeof(Fridge);

    //Cria e abre a zona de memória partilhada
	int fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		perror("failed shm_open!!! ");
		exit(EXIT_FAILURE);
	}

    //Define o tamanho da zona de memória e inicializa-a com zero
	if(ftruncate(fd, DATA_SIZE) == -1) {
		fprintf(stderr, "ftruncate() failed for %s!!!\n%s\n", SHM_NAME, strerror(errno));
		exit(EXIT_FAILURE);
	}

	//Mapeia a zona de memória. Offset a zero indica que se vai usar toda a memória desde o início
	Fridge* fridge = (Fridge*)  mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(fridge == MAP_FAILED) {
		fprintf(stderr, "mmap() failed for %s!!!\n%s\n", SHM_NAME, strerror(errno));
		exit(EXIT_FAILURE);
	}


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

    for(j=0; j<REPETICOES; j++) {
        sem_wait(sems[id]);
            if(id==0) { // Marge
                if(fridge->chips == 0 || fridge->beer==0) {
                    fprintf(stderr, "\nMarge : going to walmart.. ");
                    sleep(2);
                    buy_chips(fridge);
                    fprintf(stderr, "\nMarge : Iam back!");
                } 
                eat_and_drink(fridge);
                fprintf(stderr, "\nWe now have %d chips and %d beers\n", fridge->chips, fridge->beer);
            } else { // Hommer
                if(!(fridge->chips == 0 || fridge->beer==0)) {
                    eat_and_drink(fridge);
                    fprintf(stderr, "\nHomer : nom!nom! ..bluUurp!");
                    fprintf(stderr, "\nWe now have %d chips and %d beers\n", fridge->chips, fridge->beer);
                }
            }
        sem_post(sems[(id+1)%QTD_SEMS]);
    }

	/***** Código para o pai e filho *****/
	//Fechar os semaforos
	for(i=0; i<QTD_SEMS; i++)
		if(sem_close(sems[i]) < 0) {
			fprintf(stderr, "sem_close() failed on %s\n%s\n", sems_names[i], strerror(errno));
			exit(EXIT_FAILURE);
		}

	//Fechar a memória partilhada
	if (munmap(fridge, DATA_SIZE) == -1) {
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

void buy_chips(Fridge* fridge) {
    fridge->beer+=3;
    fridge->chips+=3;
}
void eat_and_drink(Fridge* fridge) {
    fridge->beer--;
    fridge->chips--;
}