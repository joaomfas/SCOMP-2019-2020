#include "header.h"

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

int main(void) {
	
	int i = 0;
	const int NUM_CHILDREN = 1;
	const int NUMBER_REPETITIONS = 3;
	
	const int DATA_SIZE = sizeof(circ_buffer);
	

	int fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		perror("failed shm_open!!!\n");
		exit(EXIT_FAILURE);
	}

	if(ftruncate(fd, DATA_SIZE) == -1) {
		perror("failed ftruncate!!!\n");
		exit(EXIT_FAILURE);
	}
	
	circ_buffer* buffer = (circ_buffer*)mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(buffer == MAP_FAILED) {
		perror("failed mmap!!!\n");
		exit(EXIT_FAILURE);
	}
	
	buffer->lido = 0;
	buffer->escrito = 0;
	
	int id = cria_filhos(NUM_CHILDREN);
    if (id == -1) {
        perror("Fork failed!");
        exit(EXIT_FAILURE);
    }
    
    if (id == 0) { //Código do pai
        int count = 0;
        
        for(i = 0; i <= BUFFER_SIZE; i++){
			while((buffer->escrito - buffer->lido) >= BUFFER_SIZE);
			if(i == (BUFFER_SIZE)){
				count++;
				i = 0;
			}
			
			buffer->array[i] = (count * BUFFER_SIZE) + i;
			printf("escrevi %d\n", (count * BUFFER_SIZE) + i);
			buffer->escrito++;
			
						
			if(count == NUMBER_REPETITIONS){
				break;
			}
		}

		if (munmap(buffer, DATA_SIZE) == -1)
        {
            perror("munmap failed");
            exit(EXIT_FAILURE);
        }

        if (close(fd) == -1)
        {
            perror("close failed");
            exit(EXIT_FAILURE);
        }
	
    } else { //Código dos filhos
        int temp = 0;
		for(i = 0; i <= BUFFER_SIZE; i++){
			while((buffer->escrito - buffer->lido) <= 0);
			
			if(buffer->lido == BUFFER_SIZE * NUMBER_REPETITIONS){
				break;
			}
			
			if(i == (BUFFER_SIZE)){
				i = 0;
			}
			
			temp = buffer->array[i];
			buffer->lido++;
			printf("Foi lido o valor %d\n", temp);
		}
		
        if (munmap(buffer, DATA_SIZE) == -1){
            perror("munmap failed");
            exit(EXIT_FAILURE);
        }

        if (close(fd) == -1){
            perror("close failed");
            exit(EXIT_FAILURE);
        }
        
		if (shm_unlink(SHM_NAME) < 0){
			perror("Error at unlink");
			exit(EXIT_FAILURE);
		}
		
		return 0;
    }

    //Pai espera que os filhos terminem
    int status;
    for (i = 0; i < NUM_CHILDREN; i++) {
        pid_t p = wait(&status);
        if (p == -1) {
            perror("Wait failed!");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
