/**
Write a program that creates a child process and establishes with it a communicating channel through a pipe. The
parent process should send the contents of a text file to its child through the pipe. The child should print all the
received data. The parent must wait for its child to end.
**/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

#define MAX_TEXT_SIZE 1000

const int LEITURA = 0;
const int ESCRITA = 1;

int main(int argc, char *argv[] ) {

    pid_t generatedPid;
    int twitter[2], feedback[2];
    pipe(twitter);
    pipe(feedback);
 
    generatedPid = fork();

    if (generatedPid == -1  ) return (1);

    if (generatedPid > 0){
        char vetorTwitterIn[MAX_TEXT_SIZE];
        close(twitter[LEITURA]);
        close(feedback[ESCRITA]);
        int decisao = 1;

        do{
            fflush(stdin);
            printf("\r\nESCREVA AQUI O SEU TWITTER TWITTER: \n ");
            gets(vetorTwitterIn);
            //getchar();
            decisao = write(twitter[ESCRITA], &vetorTwitterIn, sizeof(vetorTwitterIn));
            if(decisao == -1) {
                perror("write failed");
            } else {
                read(feedback[LEITURA], &decisao, sizeof(decisao));
            }
        }while(decisao >0);

        close(twitter[ESCRITA]); 
        close(feedback[LEITURA]); 

    } else {
        
        char vetorTwitterOut[MAX_TEXT_SIZE];
        int exitValue = 0;
        int verificacao = 1;
        int decisao = 1;

        close(twitter[ESCRITA]);
        close(feedback[LEITURA]);
    
        do{
            if( (read(twitter[LEITURA], vetorTwitterOut, sizeof(vetorTwitterOut))) > 0){
                printf("\n\t\t\t\t\t\t NOVA PUBLICAÇÃO NO TWITTER");
                printf("\n\n\t\t\t\t\t\t\"%s\"\n",vetorTwitterOut);

                printf("\n\n\t\t\t\t\t\tESCREVER NOVO TWITTER (1 - SIM | 0 - NAO): ");
                fflush(stdin);
                scanf("%d",&decisao);

                write(feedback[ESCRITA], &decisao, sizeof(decisao));
                
            }else{
                verificacao = -1;  
            }  
        }while(verificacao==1);

        close(twitter[LEITURA]); 
        close(feedback[ESCRITA]); 
        exit(exitValue);
    }

    int ret1_pidChild, status;
    ret1_pidChild = wait(&status);
    if ((status >> 8) == -1){
        printf("\n\n>>PROCESSO FILHO RETORNOU COM ERRO %d", ret1_pidChild, (status >> 8));
    }else{
        printf("\n\n>>ENCERRANDO TWITTER COM SUCESSO\n\n");
    }
        
    return 0;
}
