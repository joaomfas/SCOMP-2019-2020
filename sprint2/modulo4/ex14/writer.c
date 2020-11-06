// 14. Implement the following set of programs:
//-----------------------------------------------------------------------------------------
// • Reader, reads a string to a shared memory area:
// 		– The readers do not change the shared memory area, therefore 
//			several readers can in parallel access the shared memory area;
// 		– Each reader should print a string and the number of readers at the time. 
// 		– Readers can only access shared memory when there are no writers;
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
// • Writer, writes on the shared memory area:
// 		– Writes its PID and current time; 
//		– Writers have priority over readers;
//		– Only one writer can access the shared memory area at the same time; 
//		– Each writer prints the number of writers as also the number of readers. 
//-----------------------------------------------------------------------------------------
//
//	Execute several readers and writers at the same time to test your software. 
//  
//  <<WRITER>>
//
//-----------------------------------------------------------------------------------------

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
#include "ex14.h"

int main() {
	int i;

    const int QTD_SEMS = 5;
	char sems_names[QTD_SEMS][MAX_TAM_STR]; //Nomes para os semáforos, um nome em cada linha da matriz de chars
	int sems_init_values[] = {1, 1, 1, 1, 1}; //Valor inicial para cada um dos semáforos
	enum index{MUTEX1, MUTEX2, MUTEX3, WRITER, READER}; //enumerado para facilitar a leitura do código de acesso aos semáforos

    //Confirma se o número de semáforos está de acordo com o tamanho do vetor de inicializações
	if(sizeof(sems_init_values)/sizeof(sems_init_values[0]) != QTD_SEMS) {
		perror("Incorrect size for sems_init_values!!! ");
		exit(EXIT_FAILURE);
	}
    
	/***** Memória Partilhada *****/
	const int DATA_SIZE = sizeof(Dados);

	//Cria e abre a zona de memória partilhada
	int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		perror("aQUI\n\nfailed shm_open!!! ");
		exit(EXIT_FAILURE);
	}

	//Define o tamanho da zona de memória e inicializa-a com zero
	if(ftruncate(fd, DATA_SIZE) == -1) {
		fprintf(stderr, "ftruncate() failed for %s!!!\n%s\n", SHM_NAME, strerror(errno));
		exit(EXIT_FAILURE);
	}

	//Mapeia a zona de memória. Offset a zero indica que se vai usar toda a memória desde o início
	Dados* dados = (Dados*) mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(dados == MAP_FAILED) {
		fprintf(stderr, "mmap() failed for %s!!!\n%s\n", SHM_NAME, strerror(errno));
		exit(EXIT_FAILURE);
	}
    
   

	/***** SEMAFOROS *****/
	sem_t* sems[QTD_SEMS];
	for(i=0; i<QTD_SEMS; i++) {
		snprintf(sems_names[i], MAX_TAM_STR, "%s%d", SEM_NAME, i); //gera o nome para o semáforo
		
		if ((sems[i] = sem_open(sems_names[i], O_CREAT , 0644, sems_init_values[i])) == SEM_FAILED ) {
			fprintf(stderr, "sem_open() failed for %s!!!\n%s\n", sems_names[i], strerror(errno));
			exit(EXIT_FAILURE);
		}
	}


    //------ ESCRITOR    
    sem_wait(sems[MUTEX2]);
        dados->writecount++;
        if (dados->writecount == 1)
            sem_wait(sems[READER]);
    sem_post(sems[MUTEX2]);

    sem_wait(sems[WRITER]);
    //…
    //  Escrita
    sleep(3);
    printf("Escrever demora mais tempo..\n");
	printf("numero de leitores : %d!\n", dados->readcount);
	printf("numero de escritores : %d!\n", dados->writecount);
	printf("-----------------------------------------------\n");
    
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    snprintf(dados->linha[dados->cur], MAX_LINHA, "PID %d, data de escrita : %s", getpid(), asctime(timeinfo)); //gera o nome para o semáforo
    dados->cur++;

    //…
    sem_post(sems[WRITER]);

    sem_wait(sems[MUTEX2]);
        dados->writecount--;
        if (dados->writecount == 0)
            sem_post(sems[READER]);
    sem_post(sems[MUTEX2]);


	//Fechar os semaforos
	for(i=0; i<QTD_SEMS; i++)
		if(sem_close(sems[i]) < 0) {
			fprintf(stderr, "sem_close() failed on %s\n%s\n", sems_names[i], strerror(errno));
			exit(EXIT_FAILURE);
		}


	//Fechar a memória partilhada
	if (munmap(dados, DATA_SIZE) == -1) {
		perror("failed munmap! ");
		exit(EXIT_FAILURE);
	}
	if(close(fd) == -1) {
		perror("failed munmap! ");
		exit(EXIT_FAILURE);
	}

	return 0;
};