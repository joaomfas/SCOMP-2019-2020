
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

#define LEITURA 0
#define ESCRITA 1

#define FILHOS 5
#define LEN 1000


//6. Given two integer arraysvec1 and vec2,with 1000 elements each, 
//write a program that creates 5 child processes that communicate with the parent 
//through a pipe, to concurrently sum the two arrays. 
//Each child should calculate tmp+= vec1[i] + vec2[i] on 200 elements,
//then sending tmp to its parent through the pipe. Ensure that each child computes
//different positions of the arrays. The parent process should wait for the
// 5 partial sums and then calculate the final result.
//There must be only one pipe, shared by all processes.


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
    int i;
    int vec1[LEN];
    int vec2[LEN];

    // *** Preencher 1º Array *** //
    for(i=0; i<LEN; i++) {
        vec1[i] = rand() % 2 + 1;
    }
    // *** Preencher 2º Array *** //
    for(i=0; i<LEN; i++) {
        vec2[i] = rand() % 2 + 1;
    }

    int fd[2];
    //cria pipe
    if(pipe(fd) == -1) {
            perror("Pipe failed!");
            exit(EXIT_FAILURE);
    }

    int nrChild = cria_filhos(FILHOS);

    if(nrChild == 0) {
        close(fd[ESCRITA]);
        int result = 0;
        int tmp;
        for(i=0; i<FILHOS; i++) {
            int status;
            wait(&status);
            if (WIFEXITED(status))
            {
                read(fd[LEITURA], &tmp, sizeof(int));
                result += tmp;
            }
        }
            printf("resultado final = %d\n", result);
    }
    else {
        close(fd[LEITURA]);
        int numeroEnviado = 0;
        //numero de operações por filho
        int tamanhoArrayFilho = LEN/FILHOS;
        int inicio = nrChild * tamanhoArrayFilho;
        int fim = inicio + tamanhoArrayFilho;
        //cada filho soma uma parcela da array
        int j;
        for(j=inicio; j<fim; j++) {
            numeroEnviado += vec1[j] + vec2[j];
        }
        write(fd[ESCRITA], &numeroEnviado, sizeof(int));
        close(fd[ESCRITA]);
    }
    return 0;
}
