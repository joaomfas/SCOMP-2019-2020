
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

// a.Solve the exercise by first sending an integer and then a string in two separate write operations;


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

        printf("Pai: Insiria uma palavra: \n");
        scanf("%s", str);
        sleep(1);

        // 1ª operação de escrita (escreve o número)
        if(write(fd[ESCRITA], &num, sizeof(num))==-1){
            perror("Write failed!");
        }
        // 2ª operação de escrita (escreve a palavra)
        if(write(fd[ESCRITA], str, sizeof(str))==-1){
            perror("Write failed!");
        }
        close(fd[ESCRITA]);	//Fecha a extremidade usada

    }else{

        // código do filho
        close(fd[ESCRITA]);	//Fecha a extremidade NÃO usada
        // lê número
		int num;
		if(read(fd[LEITURA], &num, sizeof(num)) == -1){
			perror("Read failed!!");
			exit(EXIT_FAILURE);
		}
		printf("Filho: Retirei do pipe o numero %d\n", num);

        // lê palavra
		if(read(fd[LEITURA], str, sizeof(str)) == -1){
			perror("Read failed!!");
			exit(EXIT_FAILURE);
		}
		printf("Filho: Retirei do pipe a palvra %s\n", str);

		close(fd[LEITURA]);	//Fecha a extremidade usada
		exit(EXIT_SUCCESS);
	}
	return 0;
}
