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

#define SEMS_NOME_BASE "/sem_ex08"
#define MAX_TAM_STR 25

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

/**
 * funçao para imprimir mensagem
 */
void imprimirMsg(char *msg) {
    printf("%s", msg);
    fflush(stdout);
}


int main(int argc, char* argv[]) {
	int i;
	
	const int QTD_FILHOS = 2;
	const int QTD_SEMS = 2; //pode parecer excessivo, mas permite reutilizar o código para exercícios com mais semáforos
	char sems_names[QTD_SEMS][MAX_TAM_STR]; //Nomes para os semáforos, um nome em cada linha da matriz de chars
	enum index{SEM_printS, SEM_printC}; //enumerado para facilitar a leitura do código de acesso aos semáforos
	int sems_init_values[] = {1, 0}; //Valor inicial para cada um dos semáforos
	

	//Confirma se o número de semáforos está de acordo com o tamanho do vetor de inicializações
	if(sizeof(sems_init_values)/sizeof(sems_init_values[0]) != QTD_SEMS) {
		perror("Incorrect size for sems_init_values!!! ");
		exit(EXIT_FAILURE);
	}

	
	/***** SEMAFOROS *****/
	sem_t* sems[QTD_SEMS];
	for(i=0; i<QTD_SEMS; i++) {
		snprintf(sems_names[i], MAX_TAM_STR, "%s%d", SEMS_NOME_BASE, i); //gera o nome para o semáforo
		if ((sems[i] = sem_open(sems_names[i], O_CREAT | O_EXCL, 0644, sems_init_values[i])) == SEM_FAILED ) {
			fprintf(stderr, "sem_open() failed for %s!!!\n%s\n", sems_names[i], strerror(errno));
			exit(EXIT_FAILURE);
		}
	}


	/******** PROCESSOS *******/
	int id = cria_filhos(QTD_FILHOS); //Os processos têm de ser criados DEPOIS da memória partilhada e dos semáforos
	if(id == -1) {
		perror("fork() failed!!! ");
		exit(EXIT_FAILURE);
	}

	/*** Código dos filhos ***/
	//P1
	if(id==1){
		int value;
		for(i=0; i<30; i++){
			sem_wait(sems[SEM_printS]);
			imprimirMsg(" S");
			sem_getvalue(sems[SEM_printS],&value);
			printf(" -> O valor do sem_S: %d\n", value);
			if(value==0){
				sem_post(sems[SEM_printC]);
				sem_post(sems[SEM_printC]);
			}		
			
		}
		exit(EXIT_SUCCESS);
	}
	
	//P2
	if(id==2){
		int value;
		for(i=0; i<30; i++){
			sem_wait(sems[SEM_printC]);
			imprimirMsg(" C");
			sem_getvalue(sems[SEM_printC],&value);
			printf(" -> O valor do sem_C: %d\n", value);
			if(value==0){
				sem_post(sems[SEM_printS]);
				sem_post(sems[SEM_printS]);
			}
			
		}
		exit(EXIT_SUCCESS);
	}
	
	
	/***** Código do pai *****/
	
	if(id==0){
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

	/***** Código apenas para o pai *****/
	if(id==0) {

		//Remover os semaforos
		for(i=0; i<QTD_SEMS; i++)
			if (sem_unlink(sems_names[i]) < 0) {
				fprintf(stderr, "sem_unlink() failed on %s\n%s\n", sems_names[i], strerror(errno));
				exit(EXIT_FAILURE);
			}
	}

	return EXIT_SUCCESS;
}
