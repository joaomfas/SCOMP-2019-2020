#include "header.h"

#define MUTEX 0
#define TOTAL_SALA 1
#define VIP_ESPERA 2
#define ESPECIAL_ESPERA 3

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
	const int NUM_FILHOS = 400;
	
	const int NORMAL = 1;
	const int ESPECIAL = 2;
	const int VIP = 3;
	
	int i;
	char SEMS_NOME[NUM_SEMS][100] = {"MUTEX", "TOTAL_SALA", "VIP_ESPERA", "ESPECIAL_ESPERA"};
	const int SEMS_VALOR_INICIAL[NUM_SEMS] = {1, MAX_LUGARES, 0, 0};
	sem_t *sems[NUM_SEMS];
	
	// Memoria partilhada
    // Cria e abre a zona de memoria partilhada
    int fd;
    shared_data_type *sharedData = __shmOPEN(SHM_NAME, &fd, O_RDWR | O_CREAT);
	
	//Inicia os dados
	sharedData->vipEspera = 0;
	sharedData->especialEspera = 0;
	
	//Semaforos
	// Criação de sems
	for(i = 0; i < NUM_SEMS; i++){
		if((sems[i] = sem_open(SEMS_NOME[i], O_CREAT | O_EXCL, 0644, SEMS_VALOR_INICIAL[i])) == SEM_FAILED){
			perror("Error at sem open!\n");
			exit(EXIT_FAILURE);
		}
	}
    
    int id = spawn_childs(NUM_FILHOS);
    srand(time(NULL));
	if(id > 0){	
		int tipo_cliente = rand() % 3 + 1; //Gera número entre 1 e 3 (tipo de cliente aleatório)
		
		if(tipo_cliente == NORMAL){
			printf("Cliente normal em espera. ID: %d\n", id);
			
			int sem_value;
			sem_getvalue(sems[TOTAL_SALA], &sem_value);
			
			if(sem_value == 0){
				sem_wait(sems[MUTEX]);
				int especialEspera = sharedData->especialEspera;
				int vipEspera = sharedData->vipEspera;
				sem_post(sems[MUTEX]);
				
				if(vipEspera > 0){
					sem_wait(sems[VIP_ESPERA]);
				}else if(especialEspera > 0){
					sem_wait(sems[ESPECIAL_ESPERA]);
				}
			}
				
			sem_wait(sems[TOTAL_SALA]);
			printf("Cliente normal entrou. ID: %d\n", id);
			
			sleep(1);
			sem_post(sems[TOTAL_SALA]);
			
			printf("Cliente normal saiu da sala. ID: %d\n", id);
			
			__shmCLOSE(sharedData, fd);
			for(i = 0; i < NUM_SEMS; i++){
				__semaforo__CLOSE(sems[i]);
			}
			
			return(EXIT_SUCCESS);
			
		}else if(tipo_cliente == ESPECIAL){
			printf("Cliente especial em espera. ID: %d\n", id);
			
			int sem_value;
			sem_getvalue(sems[TOTAL_SALA], &sem_value);
			
			if(sem_value == 0){
				sem_wait(sems[MUTEX]);
				int vipEspera = sharedData->vipEspera;
				sem_post(sems[MUTEX]);
				
				if(vipEspera > 0){
					sem_wait(sems[VIP_ESPERA]);
				}
			}
			
			sem_wait(sems[TOTAL_SALA]);
			
			sem_wait(sems[MUTEX]);
			int especialEspera = sharedData->especialEspera;
			sem_post(sems[MUTEX]);
			if(especialEspera == 0){
				sem_post(sems[ESPECIAL_ESPERA]);
			}
			
			printf("Cliente especial entrou. ID: %d\n", id);
			
			sleep(1);
			sem_post(sems[TOTAL_SALA]);
			
			printf("Cliente especial saiu da sala. ID: %d\n", id);
			
			__shmCLOSE(sharedData, fd);
			for(i = 0; i < NUM_SEMS; i++){
				__semaforo__CLOSE(sems[i]);
			}
			
			return(EXIT_SUCCESS);
		}else if(tipo_cliente == VIP){
			printf("Cliente VIP em espera. ID: %d\n", id);
			
			sem_wait(sems[TOTAL_SALA]);
			
			sem_wait(sems[MUTEX]);
			int especialEspera = sharedData->vipEspera;
			sem_post(sems[MUTEX]);
			if(especialEspera == 0){
				sem_post(sems[VIP_ESPERA]);
			}
			
			printf("Cliente VIP entrou. ID: %d\n", id);
			
			sleep(1);
			sem_post(sems[TOTAL_SALA]);
			
			printf("Cliente VIP saiu da sala. ID: %d\n", id);
			
			__shmCLOSE(sharedData, fd);
			for(i = 0; i < NUM_SEMS; i++){
				__semaforo__CLOSE(sems[i]);
			}
			
			return(EXIT_SUCCESS);
		}
	}
	
	//Espera que os filhos terminem
	for (i = 0; i < NUM_FILHOS; i++){
		wait(NULL);
	}
	
	//Fechar e apagar memória partilhada
	__shmCLOSE(sharedData, fd);
	__shmDELETE(SHM_NAME);
	
	//Fechar e remover semáforos
	for(i = 0; i < NUM_SEMS; i++){
		__semaforo__CLOSE(sems[i]);
		__semaforo__UNLINK(SEMS_NOME[i]);
	}
	
	return 0;
}
