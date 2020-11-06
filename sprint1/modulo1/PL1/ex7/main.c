#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 1000

int main(void) {
    int count=0;
    int status;
    int numbers[ARRAY_SIZE];                                    /* array to lookup */
    int n;                                                      /* the number to find */
    time_t t;                                                   /* needed to init. the random number generator (RNG)*/
    int i;                                                      /* intializes RNG (srand():stdlib.h; time(): time.h) */
    srand ((unsigned) time (&t));                               /* initialize array with random numbers (rand(): stdlib.h) */
    for (i = 0; i < ARRAY_SIZE; i++){
        numbers[i] = rand () % 10000;                           /* initialize n */
    }
    n = rand () % 10000;

    pid_t f=fork();

    if(f==0){
        for(i=(ARRAY_SIZE/2);i<(ARRAY_SIZE-1);i++){
            if(numbers[i] == n){
                count++;
            }
        }
        exit(count);
    }else{
        for(i=0;i<(ARRAY_SIZE/2-1);i++){
            if(numbers[i] == n){
                count++;
            }
        }
    }

    int count_total=count;
    pid_t p;
    p=waitpid(f,&status,0);
    if(p==-1){
        perror("wait failled!");
        exit(EXIT_FAILURE);
    }else if(WIFEXITED(status)){
        count_total+=WEXITSTATUS(status);
        printf("O valor %d foi encontrado %d vezes\n", n, count_total);
    }
    return 0;
}

