
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

#define FILHOS 6
// a) Ensure that the function returns to the child process 
// an index (1 for the first child process created, 2 for the 
// second, 3 for the third and so on) and that to the parent 
// process returns 0. 
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

// b) Write a program that uses the function spawn_childs() 
// to create 6 child processes. Each child process should exit 
// returning its index value * 2. The parent should wait for 
// all processes created. 
int main() {
    int status;
    int index;
    index = spawn_childs(FILHOS);
    if(index > 0) {
        exit(index << 1);
    }

    //pai espera por filhos todos os filhos
    int i;
    for(i=0; i<FILHOS; i++) {
        pid_t p;
        p = wait(&status);
        if(p==-1){
            perror("wait failled!");
            exit(EXIT_FAILURE);
        } else if(WIFEXITED(status)){
            printf("Filho return : %d\n", WEXITSTATUS(status));
        }
    }
    return 0;
}