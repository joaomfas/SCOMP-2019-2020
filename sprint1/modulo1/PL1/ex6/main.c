#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void) {
    int i;
    int status;
    int count=0;
    pid_t pids[4];
    for (i = 0; i < 4; i++) {
        pid_t x=fork();
        if (x == 0) {
            sleep(1); /*sleep(): unistd.h*/
            printf("Filho %d criado em %d lugar\n",getpid(), i+1);
            exit(i+1);
        }else {
            if(x%2==0){
                pids[count]=x;
                count++;
            }
        }
    }

    for(i=0;i<count;i++){
        waitpid(pids[i],&status,0);
        printf("Esperei pelo filho %d\n", pids[i]);
    }

    printf("This is the end.\n");
    return 0;
}

