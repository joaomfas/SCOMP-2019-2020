
/**
 Write a program that creates 10 child processes to play the game “Win the pipe!”. There must be only one pipe,
shared by all processes. The game’s rules are as follow:
    a. The parent process fills, each 2 seconds, a structure with a message “Win” and the round’s number (1 to 10) and writes this data in the pipe;
    b. Each of child processes is trying to read data from the pipe. The child that succeeds should print the
        winning message and the round’s number, ending its execution with an exit value equal to the winning round’s number;
    c. The remaining child processes continue to try to read data from the pipe;
    d. After all child processes terminate, the parent process should print the PID and the winning round of each child. 
**/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_TEXT_SIZE 1000
#define NUM_FILHOS 10

const int LEITURA = 0;
const int ESCRITA = 1;

typedef struct {
    char msg[10];
    int round;
} MessagePacket;

typedef struct {
    int pidVencedor;
    int round;
} Ranking;

int main(int argc, char *argv[] ) {

    pid_t generatedPid;
    int feedback[2];
    pipe(feedback);


    for (int i=0; i < NUM_FILHOS; ++i) {
        generatedPid = fork();
        if (generatedPid == -1) return (1);
        if (generatedPid==0) break;
    }
    
    if(generatedPid == 0) {
        MessagePacket pacote;

        int exitValue = 0;

        close(feedback[ESCRITA]);
    
        read(feedback[LEITURA], &pacote, sizeof(pacote));
        printf("Filho: %d %s - Round: %d\n",getpid(), pacote.msg, pacote.round);
        
        exitValue = pacote.round;
        close(feedback[LEITURA]); 
        exit(exitValue);

    } else {
        MessagePacket pacote;
        strcpy(pacote.msg, "win");

        close(feedback[LEITURA]);

        for (int i=0; i < NUM_FILHOS; ++i) {
            pacote.round = i+1;
            write(feedback[ESCRITA], &pacote, sizeof(pacote));
            sleep(2);
        };

        close(feedback[ESCRITA]); 

    } 

    Ranking listaVencedores[NUM_FILHOS];

    for (int i=0; i < NUM_FILHOS; ++i) {
        int ret1_pidChild, status;
        ret1_pidChild = wait(&status);
        if ((status >> 8) == -1){
             listaVencedores[i].pidVencedor = ret1_pidChild;
             listaVencedores[i].round = (status >> 8);
            printf("\n\n>>PROCESSO FILHO %d RETORNOU COM ERRO %d", ret1_pidChild, (status >> 8));
        }else{
             listaVencedores[i].pidVencedor = ret1_pidChild;
             listaVencedores[i].round = (status >> 8);
        }
    }

    printf("\nRanking:\n");
    for (int i=0; i < NUM_FILHOS; ++i) {
         printf("\nFilho %d venceu o %dº round.", listaVencedores[i].pidVencedor,  listaVencedores[i].round);
    }
    
        
    return 0;
}
