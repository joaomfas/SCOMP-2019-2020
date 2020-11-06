
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


// 7.Given two integer arrays vec1 and vec2, 
// with 1000 elements each, write a program that creates 
// 5 child processes to concurrently sum the two arrays. 
// Each child should calculate vec1[i] + vec2[i] on 200 
// elements, sending all those values to its parent. 
// Ensure that each child computes different positions of 
// the arrays. The parent process should wait for all the 
// 1000 values and then store them in a result array, in 
// the correct position. Use 5 pipes, one for each child


int spawn_childs(int n) {
    int i;
    for (i=1; i<=n; i++) {
        pid_t pid = fork();
        if(pid==-1){
            perror("wait failled!");
            exit(EXIT_FAILURE);
        } 
        if(pid==0) {
            return i;
        }
    }
    return 0;
}

int main() {
    int i;
    int vec1[LEN];
    int vec2[LEN];
    int result[FILHOS];
    

    // *** Preencher * Array *** //
    for(i=0; i<LEN; i++) {
        vec1[i] = rand() % 2 + 1;
        vec2[i] = rand() % 2 + 1;
    }

    
    int fd[FILHOS][2];
    for(i=0; i<FILHOS; i++) {
        if(pipe(fd[i]) == -1) {//cria pipe
            perror("Pipe failed!");
            exit(EXIT_FAILURE);
        }
    }

    int nthChild = spawn_childs(FILHOS);

    if(nthChild == 0) {
        for(i=0; i<FILHOS; i++) {
            int numeroRecebido;
            close(fd[i][ESCRITA]);
            read(fd[i][LEITURA], &numeroRecebido, sizeof(int));
            close(fd[i][LEITURA]);
            result[i] = numeroRecebido;
            printf("result[%d] : %d\n", i, result[i]);
        }
    }
    else {
        int numeroEnviado = 0;      
        //numero de operações por filho
        int tamanhoArrayFilho = LEN/FILHOS;
        int inicio = (nthChild-1) * tamanhoArrayFilho;
        int fim = (nthChild) * tamanhoArrayFilho;
        //cada filho soma uma parcela da array
        int j;
        for(j=inicio; j<fim; j++) {
            numeroEnviado += vec1[j] + vec2[j];
        }
        close(fd[nthChild-1][LEITURA]);
        write(fd[nthChild-1][ESCRITA], &numeroEnviado, sizeof(int));
        close(fd[nthChild-1][ESCRITA]);
        printf("Father..my job here is done.. where's ma money?\n");
        exit(EXIT_SUCCESS);
    }
    //Espera pelos filhos
    for(i=0; i<FILHOS; i++) {
        wait(NULL);
        printf("kids these days...\n");
    }
    return 0;
}