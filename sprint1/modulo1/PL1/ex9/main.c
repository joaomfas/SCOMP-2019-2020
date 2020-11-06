#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

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

int main(void) {
    const int QTD_FILHOS = 10;
    int i;
    int id = 0;
    id = cria_filhos(QTD_FILHOS);
    if(id==-1){
        perror("fork failled!!!");
        exit(EXIT_FAILURE);
    }

    // código do filho
    if(id>0){
        for(i=((id-1)*100+1);i<id*100;i++){
            printf("Filho %d : nº %d\n", id, i);
        }
        exit(id);
    }

    // código do pai
    if(id==0){
        int status;
        pid_t p;
        for(i=0;i<QTD_FILHOS;i++){
            wait(&status);
            if(p==-1){
                perror("Waited failled!!");
                exit(EXIT_FAILURE);
            }else if(WIFEXITED(status)){
			    printf("Filho: %d\n", WEXITSTATUS(status));
		    }
        }
    }
    return 0;
}

