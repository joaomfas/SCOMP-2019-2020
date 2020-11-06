
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

#define LEITURA 0
#define ESCRITA 1

#define BUFFER 80

// 3.Write a program that creates a child process and establishes with 
// it a communication channel through a pipe. The parent process should 
// send two messages, “Hello World” and “Goodbye!”, and then closes the 
// communication channel. The child process should wait for data and print 
// it as soon as it is available, terminating after all data is received. 
// The parent process must wait for its child to end, 
// printing its PID and exit value.


int main() {
    int fd[2];
    int status;

    if(pipe(fd) == -1) {//cria pipe
        perror("Pipe failed!");
        exit(EXIT_FAILURE);
    }

    pid_t p=fork();

    if(p > 0) { //Pai
        char msg1[BUFFER] = "Hello World";
        char msg2[BUFFER] = "Good Bye";

        close(fd[LEITURA]);
        
        printf("Mensagem enviada : %s\n", msg1);
        write(fd[ESCRITA], msg1, BUFFER);

        printf("Mensagem enviada : %s\n", msg2);
        write(fd[ESCRITA], msg2, BUFFER);
        
        close(fd[ESCRITA]);
        
    } else { //Filho
        char mensagemRecebida1[BUFFER];
        char mensagemRecebida2[BUFFER];
        close(fd[ESCRITA]);
        
        read(fd[LEITURA], &mensagemRecebida1, BUFFER);
        printf("Mensagem recebida : %s\n", mensagemRecebida1);
        
        read(fd[LEITURA], &mensagemRecebida2, BUFFER);
        printf("Mensagem recebida : %s\n", mensagemRecebida2);

        close(fd[LEITURA]);
        exit(2);
    }

    //Espera pelo filho
    p=waitpid(p, &status, 0);
    if(p==-1){
        perror("wait failled!");
        exit(EXIT_FAILURE);
    }else if(WIFEXITED(status)){
        printf("Filho pid %d returns : %d \n", p, WEXITSTATUS(status));
    }
    wait(NULL);
    return 0;
}