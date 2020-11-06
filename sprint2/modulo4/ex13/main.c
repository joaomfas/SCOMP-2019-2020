// 13.Implement a program that starts by creating two new processes. 
// Each of the child processes will have the role of producer,  while  
// the  father  will  act  as  a  consumer.  Between  them  there  a  
// circular  buffer  capable  of  handling  10 integers on a shared memory area. 
// Each of the producer writes increasing values into the buffer, which should 
// be printed by all. Assume that only 30 values are exchanged and that a semaphore 
// controls the buffer access: 
//
//      • producer blocks if the buffer is full; 
//
//      • producer only writes after guaranteeing mutual exclusion;
//
//      • consumer blocks until there are no new data to read.
//

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
#include "ex13.h"

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

	const int QTD_FILHOS = 2;

	const int QTD_SEMS = 3;
	char sems_names[QTD_SEMS][MAX_TAM_STR]; //Nomes para os semáforos, um nome em cada linha da matriz de chars
	int sems_init_values[] = {0, 10, 1}; //Valor inicial para cada um dos semáforos
	enum index{FULL, EMPTY, MUTEX}; //enumerado para facilitar a leitura do código de acesso aos semáforos

	//Confirma se o número de semáforos está de acordo com o tamanho do vetor de inicializações
	if(sizeof(sems_init_values)/sizeof(sems_init_values[0]) != QTD_SEMS) {
		perror("Incorrect size for sems_init_values!!! ");
		exit(EXIT_FAILURE);
	}

	/***** Memória Partilhada *****/
	const int DATA_SIZE = sizeof(CircularBuffer);

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
	CircularBuffer* circularBuffer = (CircularBuffer*) mmap(NULL, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(circularBuffer == MAP_FAILED) {
		fprintf(stderr, "mmap() failed for %s!!!\n%s\n", SHM_NAME, strerror(errno));
		exit(EXIT_FAILURE);
	}
    circularBuffer->consumption=0;
    circularBuffer->head=0;
    circularBuffer->tail=0;
	circularBuffer->buffer=0;
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
	int id = cria_filhos(QTD_FILHOS); //Os processos têm de ser criados DEPOIS da memória partilhada e dos semáforos
	if(id == -1) {
		perror("fork() failed!!! ");
		exit(EXIT_FAILURE);
	}

    while (circularBuffer->consumption < MAX_CONSUMPTION) {

		
		if(id==0) { // pai | consumidores
			usleep(10*1000);// <- para testar 
			sem_wait(sems[FULL]);
			sem_wait(sems[MUTEX]);
			//• consumer blocks until there are no new data to read.
			while(circularBuffer->buffer>0) 
				consume(circularBuffer);
			//printf("PAI : %d \n", id);
			sem_post(sems[MUTEX]);
			int k;
			for(k=0; k<QTD_FILHOS; k++)//desbloqueia todos os produtores
				sem_post(sems[EMPTY]);
			
		} else { // filhos | produtores
			sem_wait(sems[EMPTY]);
			sem_post(sems[MUTEX]);
			//printf("FILHO : %d \n", id);
			produce(circularBuffer);
			sem_post(sems[MUTEX]);
			sem_post(sems[FULL]);
		}
		printcb(circularBuffer);
    }

	if(id==0) { /***** Código do pai (escritor) *****/
		//Esperar que os filhos terminem (neste caso é apenas um filho, mas facilita a reutilização para exs com mais filhos)
		int status;
		for(i=0; i<QTD_FILHOS; i++) {
			wait(&status);
			if(WEXITSTATUS(status) != EXIT_SUCCESS) {
				perror("Filho terminou com erro! ");
				exit(EXIT_FAILURE);
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
	if (munmap(circularBuffer, DATA_SIZE) == -1) {
		perror("failed munmap! ");
		exit(EXIT_FAILURE);
	}
	if(close(fd) == -1) {
		perror("failed munmap! ");
		exit(EXIT_FAILURE);
	}


	/***** Código apenas para o pai *****/
	if(id==0) {
		//Remover a memória partilhada
		if(shm_unlink(SHM_NAME) == -1) {
			perror("failed shm_unlink! ");
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
};

void produce(CircularBuffer* circularBuffer){
	int producedValue = rand()%10+5; // valor entre [5,15]
	circularBuffer->number[circularBuffer->tail] = producedValue;
	circularBuffer->tail=(circularBuffer->tail+1)%BUFFER;
	circularBuffer->buffer++;
	fprintf(stderr,"[+] : %d!\n", producedValue);
};
void consume(CircularBuffer* circularBuffer){
	fprintf(stderr,"[-] : %d!\n", circularBuffer->number[circularBuffer->head]);
	circularBuffer->head=(circularBuffer->head+1)%BUFFER;
	circularBuffer->buffer--;
	circularBuffer->consumption++;
};
void printcb(CircularBuffer* circularBuffer){
	int inicio = circularBuffer->head;
	int fim = circularBuffer->tail;
	int contador = 0;
	while(inicio!=fim) {
		fprintf(stderr,"buffer[%d] = %d\n", contador, circularBuffer->number[inicio]);
		inicio = (inicio + 1)%BUFFER;
		contador++;
	}
	fprintf(stderr,"------------------------------\n");
}