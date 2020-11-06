#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

typedef struct Sale{
     int customer_code;
     int product_code;
     int quantity;
} Sale;

int cria_filhos(int n){
	pid_t pid;
	
	int i;
	for(i = 0; i < n; i++){
		pid = fork();
		if(pid < 0){
			return -1;
		}else if(pid == 0){
			return i+1;
		}
	}
	return 0;
}

int main(void){
	const int MAX_SALES = 50000;
	const int NUM_CHILDREN = 10;
	const int BUFFER_SIZE = sizeof(Sale);
	Sale sales[MAX_SALES];
	int fd[2];
	enum extremidade{LEITURA=0, ESCRITA=1};
	
	int i;
	for (i = 0; i < MAX_SALES; i++) {
        Sale sale;
        sale.customer_code = i;
        sale.product_code = i;
        sale.quantity = 2;
        sales[i] = sale;
    }
    sales[30].quantity = 500;
    sales[900].quantity = 500;
	
	if(pipe(fd) == -1){
		perror("Pipe failed!");
		exit(EXIT_FAILURE);
	}
	
	int id = cria_filhos(NUM_CHILDREN);
	if(id == -1){
		perror("Fork failed!");
		exit(EXIT_FAILURE);
	}
	
	if(id == 0){ //Código do pai
		 close(fd[ESCRITA]);
        Sale sale;

        int n;
        while((n = read(fd[LEITURA], &sale, BUFFER_SIZE)) > 0) {
            printf("Produto com mais de 20 vendas: %d\n", sale.product_code);
        }

        close(fd[LEITURA]);
	}else{ //Código do filho
		close(fd[LEITURA]);
        int i;
        int inicio = (id - 1) * 5000;
        int fim = id * 5000;
        for (i = inicio; i < fim; i++) {
            int soldQuantity = sales[i].quantity;
            if (soldQuantity > 20) {
                write(fd[ESCRITA], &sales[i], BUFFER_SIZE);
            }
        }
        close(fd[ESCRITA]);
        exit(EXIT_SUCCESS);
	}
	
	//Pai espera que os filhos terminem
	int status;
	for(i = 0; i < NUM_CHILDREN; i++){
		pid_t p = wait(&status);
		if(p == -1){
			perror("Wait failed!");
			exit(EXIT_FAILURE);
		}
	}
	
	return 0;
}
