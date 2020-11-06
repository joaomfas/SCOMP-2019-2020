#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 1000

int main(void) {
    pid_t p;
    if (fork() == 0) {
        printf("PID = %d\n", getpid());
        //sleep(20);
        exit(0);
    }
    if ((p=fork()) == 0) {
        printf("PID = %d\n", getpid()); exit(0);
    }
    printf("Parent PID = %d\n", getpid());
    printf("Waiting... (for PID=%d)\n",p);
    waitpid(p, NULL, 0);
    printf("Enter Loop...\n");
    while (1);                                              /* Infinite loop */
}

