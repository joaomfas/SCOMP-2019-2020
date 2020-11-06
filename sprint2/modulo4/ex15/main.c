// 15. Consider a show room and a set of visitors. Each visitor is simulated 
// by an infinity loop in which he enters the show room and leaves after a while. 
// Each show in a room starts and ends at defined times, the visitors can 
// enter and leaving the room at those times. Each room supports 5 visitors 
// if more want to enter the room then they have to wait.

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
#include "ex15.h"

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
	int i, j;
	const int REPETICOES = 3;
	const int QTD_SEMS = 4;
	char sems_names[QTD_SEMS][MAX_TAM_STR]; //Nomes para os semáforos, um nome em cada linha da matriz de chars
	int sems_init_values[] = {0, 0, 1, 0}; //Valor inicial para cada um dos semáforos
	enum index{IN, OUT, MUTEX, PLAYING}; //enumerado para facilitar a leitura do código de acesso aos semáforos

	//Confirma se o número de semáforos está de acordo com o tamanho do vetor de inicializações
	if(sizeof(sems_init_values)/sizeof(sems_init_values[0]) != QTD_SEMS) {
		perror("Incorrect size for sems_init_values!!!\n");
		exit(EXIT_FAILURE);
	}


	/***** Memória Partilhada *****/
	const int DATA_SIZE = sizeof(Show);

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
	Show* show = (Show*) mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(show == MAP_FAILED) {
		fprintf(stderr, "mmap() failed for %s!!!\n%s\n", SHM_NAME, strerror(errno));
		exit(EXIT_FAILURE);
	}

	show->seats=0;
	show->terminou=0;
	show->fila=0;

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

	while(!show->terminou) {
		if(id==0) { // responsável por criar sessões periódicas.
			for(j=0; j<REPETICOES; j++) {
				// liberta 5 lugares
				for(i=0; i<MAX_SEATS; i++)
					sem_post(sems[IN]);
				fprintf(stderr,"\n\n [ Anuncios ] \n\n"); //<- começam os anuncios
				
				sem_wait(sems[PLAYING]); //<- espera que todos entrem

				fprintf(stderr,"\n\n SESSÃO [%d] : FILME COMEÇA\n", j+1);
                fprintf(stderr,"\n PLAYING : JOKER \n");
				sleep(SHOW_TIME);
				// Filme acaba -> quem está dentro sai! + intervalo entre sessões
				fprintf(stderr,"\n SESSÃO [%d] : FILME ACABA\n", j+1);
				
				for(i=0; i<show->seats; i++) {
					sem_post(sems[OUT]);
				}

				sleep(SHOW_INTERVAL);

				if(j==REPETICOES-1) {//terminou por hj, toda a gente vai para casa
					sem_wait(sems[MUTEX]);//escreve para memoria partilhada
						fprintf(stderr,"\n Sessões acabaram por hoje : TODA A GENTE EMBORA!\n");
						show->terminou=1;
						
						//manda todos embora
						while(show->fila>0) {
							show->fila--;
							sem_post(sems[IN]);							
						}
					sem_post(sems[MUTEX]);
				}
			}
		} else {
			while(!show->terminou) {
				//printf("\n [%d] vou para a fila..", id);
				sem_wait(sems[MUTEX]);
					fprintf(stderr,"\n [%d] vou para a fila..", id);
					show->fila++;
				sem_post(sems[MUTEX]);
					sem_wait(sems[IN]);
				sem_wait(sems[MUTEX]);		
					show->fila--;
					if(!show->terminou) {
						show->seats++;	
						fprintf(stderr,"\n [%d] Consegui lugar! %d/%d seats", id, show->seats,MAX_SEATS);
						if(show->seats==MAX_SEATS) {
							sem_post(sems[PLAYING]);
						}	
						sem_post(sems[MUTEX]);
							sem_wait(sems[OUT]); // sessão terminou vai sair
							fprintf(stderr,"\n [%d] não sei porque é que toda a gente recomenda este filme, nem sequer teve piada. ", id);
						sem_wait(sems[MUTEX]);
							show->seats--;
					} else {
						fprintf(stderr, " [%d] [...] as sessões já terminaram por hoje, vou para casa!\n", id);
					}
				sem_post(sems[MUTEX]);
			}
		}

	}

	/***** Código para o pai e filho *****/
	//Fechar os semaforos
	for(i=0; i<QTD_SEMS; i++)
		if(sem_close(sems[i]) < 0) {
			fprintf(stderr, "sem_close() failed on %s\n%s\n", sems_names[i], strerror(errno));
			exit(EXIT_FAILURE);
		}

	//Fechar a memória partilhada
	if (munmap(show, DATA_SIZE) == -1) {
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