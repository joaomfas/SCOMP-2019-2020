
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <string.h>

#define FILHOS 1
#define BUFFER_SIZE 100


// 2. Write a program that creates a child process and establishes with it a communication channel through a pipe.
// The parent process reads an integer and a string from stdin and sends both through the pipe.
// The child process should read those values from the pipe and print them.

// b. Solve the exercise by using a structure to send both values in one write operation.




// cria filhos
int cria_filhos(int n){
    pid_t pid;
    int i;
    for(i=0;i<n;i++){
        pid=fork();
        if(pid<0){
            return -1;
        }else if(pid==0){
            return i+1;
        }
    }
    return 0;
}

int main() {
    int fd[2];
    int num;
    char str[BUFFER_SIZE];
    enum extremidade{LEITURA = 0, ESCRITA = 1};

    struct S1 {
        int numero;
        char strg[BUFFER_SIZE];
    };

    if(pipe(fd) == -1){
        perror("Pipe failed!");
        exit(EXIT_FAILURE);
    }

    // chamada da função para criar filhos
    int id = cria_filhos(FILHOS);
    if(id == -1){
		perror("Fork failed!");
		exit(EXIT_FAILURE);
	}

    // código do pai
    if(id == 0) {
        close(fd[LEITURA]);	//Fecha a extremidade NÃO usada

        printf("Pai: Insira um nº: \n");
        scanf("%d",&num);
        sleep(1);

        printf("Pai: Insira uma palavra: \n");
        scanf("%s", str);
        sleep(1);

        // guarda os 2 valores numa estrutura
        struct S1 s_pai;
        s_pai.numero=num;
        strcpy(s_pai.strg, str);

        // escreve numa única operação de escrita os valores guardados na estrutura
        if(write(fd[ESCRITA], &s_pai, sizeof(s_pai))==-1){
            perror("Write failed!");
        }
        close(fd[ESCRITA]);	//Fecha a extremidade usada

    }else{

        // código do filho
        close(fd[ESCRITA]);	//Fecha a extremidade NÃO usada
        struct S1 s_filho;
        // lê o conteúdo da estrutura
		if(read(fd[LEITURA], &s_filho, BUFFER_SIZE) == -1){
			perror("Read failed!!");
			exit(EXIT_FAILURE);
		}
        close(fd[LEITURA]);	//Fecha a extremidade usada
		printf("Filho: Retirei do pipe o numero %d\n", s_filho.numero);
		printf("Filho: Retirei do pipe a palvra %s\n", s_filho.strg);
		exit(EXIT_SUCCESS);
	}
	return 0;
}
