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

#define SEMS_NOME_BASE "/sem_ex11_"
#define MAX_TAM_STR 25
#define MAX_CAPACITY 200


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

int main(int argc, char* argv[]) {
	int i;
	
	const int QTD_FILHOS = 3;
	const int QTD_SEMS = 4;
	char sems_names[QTD_SEMS][MAX_TAM_STR]; //Nomes para os semáforos, um nome em cada linha da matriz de chars
	int sems_init_values[] = {MAX_CAPACITY, 1, 1, 1}; //Valor inicial para cada um dos semáforos
	enum index{SEM_PASS, SEM_PORTA1, SEM_PORTA2, SEM_PORTA3}; //enumerado para facilitar a leitura do código de acesso aos semáforos

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
	
	/******** CONTROLO TEMPO *******/
    int max_time = 60000; 
    clock_t start_time = clock();
	
	
	/******** PROCESSOS *******/
	int id = cria_filhos(QTD_FILHOS); //Os processos têm de ser criados DEPOIS da memória partilhada e dos semáforos
	if(id == -1) {
		perror("fork() failed!!! ");
		exit(EXIT_FAILURE);
	}

	
	/*** Código dos filhos ***/
	// Process 1 == Porta 1
	if(id==1){
			
		srand((unsigned) time(NULL));

        while (clock() < start_time + max_time)
        {
			// gera número aleatório entre 1 e 2
            int random = (rand() % 2) +1;
			
			// 1->entra passageiro
            if (random == 1)
            {
                // espera que porta seja aberta
                sem_wait(sems[SEM_PORTA1]);
                // decrementa o número de passageiros que estão fora, aumentando em 1 passageiro dentro do comboio
				// com o sem_trywait() o semáforo não fica bloqueado
                sem_trywait(sems[SEM_PASS]);
                // abre a porta
                sem_post(sems[SEM_PORTA1]);
                printf("Abriu a porta 1 => Entrou um passageiro!\n");
                usleep(100);
            }
			// 2->sai passageiro
            else
            {
                // fecha a porta
                sem_wait(sems[SEM_PORTA1]);
                int passageiros;
                // se os passageiros estiverem todos fora do comboio (os 200), então não há ninguém para sair
                sem_getvalue(sems[0], &passageiros);
                if(passageiros==MAX_CAPACITY){
                    printf("Comboio está vazio!\nNão sai ninguém nesta paragem!\n");
                    // abre a porta
					sem_post(sems[SEM_PORTA1]);
                    usleep(100);
                }else{
                    // incrementa o número de passageiros dentro do comboio, diminuindo em 1 passageiro fora do comboio
					sem_post(sems[SEM_PASS]);
					// abre a porta
					sem_post(sems[SEM_PORTA1]);
					printf("Abriu a porta 1 => Saiu um passageiro!\n");
					usleep(100);
                }
            }
        }
        exit(EXIT_SUCCESS);		
	}
	
	// Process 2 == Porta 2
	if(id==2){
			
		srand((unsigned) time(NULL));

        while (clock() < start_time + max_time)
        {
			// gera número aleatório entre 1 e 2
            int random = (rand() % 2) +1;
			
			// 1->entra passageiro
            if (random == 1)
            {
                // espera que porta seja aberta
                sem_wait(sems[SEM_PORTA2]);
                // decrementa o número de passageiros que estão fora, aumentando em 1 passageiro dentro do comboio
				// com o sem_trywait() o semáforo não fica bloqueado
                sem_trywait(sems[SEM_PASS]);
                // abre a porta
                sem_post(sems[SEM_PORTA2]);
                printf("Abriu a porta 2 => Entrou um passageiro!\n");
                usleep(100);
            }
			// 2->sai passageiro
            else
            {
                // fecha a porta
                sem_wait(sems[SEM_PORTA2]);
                int passageiros;
                // se os passageiros estiverem todos fora do comboio (os 200), então não há ninguém para sair
                sem_getvalue(sems[0], &passageiros);
                if(passageiros==MAX_CAPACITY){
                    printf("Comboio está vazio!\nNão sai ninguém nesta paragem!\n");
                    // abre a porta
					sem_post(sems[SEM_PORTA2]);
                    usleep(100);
                }else{
                    // incrementa o número de passageiros dentro do comboio, diminuindo em 1 passageiro fora do comboio
					sem_post(sems[SEM_PASS]);
					// abre a porta
					sem_post(sems[SEM_PORTA2]);
					printf("Abriu a porta 2 => Saiu um passageiro!\n");
					usleep(100);
                }
            }
        }
        exit(EXIT_SUCCESS);		
	}
	
	// Process 3 == Porta 3
	if(id==2){
			
		srand((unsigned) time(NULL));

        while (clock() < start_time + max_time)
        {
			// gera número aleatório entre 1 e 2
            int random = (rand() % 2) +1;
			
			// 1->entra passageiro
            if (random == 1)
            {
                // espera que porta seja aberta
                sem_wait(sems[SEM_PORTA3]);
                // decrementa o número de passageiros que estão fora, aumentando em 1 passageiro dentro do comboio
				// com o sem_trywait() o semáforo não fica bloqueado
                sem_trywait(sems[SEM_PASS]);
                // abre a porta
                sem_post(sems[SEM_PORTA3]);
                printf("Abriu a porta 3 => Entrou um passageiro!\n");
                usleep(100);
            }
			// 2->sai passageiro
            else
            {
                // fecha a porta
                sem_wait(sems[SEM_PORTA3]);
                int passageiros;
                // se os passageiros estiverem todos fora do comboio (os 200), então não há ninguém para sair
                sem_getvalue(sems[0], &passageiros);
                if(passageiros==MAX_CAPACITY){
                    printf("Comboio está vazio!\nNão sai ninguém nesta paragem!\n");
                    // abre a porta
					sem_post(sems[SEM_PORTA3]);
                    usleep(100);
                }else{
                    // incrementa o número de passageiros dentro do comboio, diminuindo em 1 passageiro fora do comboio
					sem_post(sems[SEM_PASS]);
					// abre a porta
					sem_post(sems[SEM_PORTA3]);
					printf("Abriu a porta 3 => Saiu um passageiro!\n");
					usleep(100);
                }
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

	return 0;
}
