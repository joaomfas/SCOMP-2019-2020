
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

//10. Write a program that simulates a betting system. Assume that the child process starts the game with a credit of 20 euros. The game has the following rules:
//a.The parent process generates a random number between 1 and 5;
//b.Then,  it  writes  1  in  the  pipe,  notifying  the  child  that  it  can  make  another  bet,  or  0,if the child’s credit ended;
//c.The child process reads the number sent by the parent process and makes a bet or terminates, accordingly. To  make  a  bet,  the  child  should  generate  a  random  number between  1  and  5  and  send  it  to  the parent process;
//d.The parent process waits for the child’s betor by its termination, accordingly. If the child’s bet is right, its credit grows 10 euros. If the bet is wrong,its credit diminishes 5 euros;
//e.The parent process sends the new credit’s value to the child process.The child process prints it.

#define LEITURA 0
#define ESCRITA 1
#define CREDITO_INICIAL 20
#define GANHA 10
#define PERDE -5
#define BUFFER_SIZE sizeof(int)

int main() {

    // constantes
    const int CONTINUA = 1;
    const int TERMINA = 0;

    // descritores (0 stdin, 1 stdout, 2 stderr)
    int fd1[2];
    int fd2[2];

    // Criar 2 pipes para ter comunicação nos 2 sentidos
    if (pipe(fd1) == -1) {
        perror("Pipe 1 failed!");
        exit(EXIT_FAILURE);
    }

    if (pipe(fd2) == -1) {
        perror("Pipe 2 failed!");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed!");
        exit(EXIT_FAILURE);
    }

    // Código do pai
    if (pid > 0) {
        srand(time(NULL));

        close(fd1[ESCRITA]);
        close(fd2[LEITURA]);
        int creditos_filho = CREDITO_INICIAL;
        while (creditos_filho > 0) {
            //nr aleatório entre 1 e 5
            int nr_aleatorio = (rand() % 5) + 1;
            printf("Nr aleatório gerado = %d\n", nr_aleatorio);
            write(fd2[ESCRITA], &CONTINUA, BUFFER_SIZE);
            int aposta;
            read(fd1[LEITURA], &aposta, BUFFER_SIZE);
            if (aposta == nr_aleatorio) {
                printf("A aposta foi de %d. Ganhou!\n", aposta);
                creditos_filho = creditos_filho + GANHA;
            } else {
                printf("A aposta foi de %d. Perdeu!\n", aposta);
                creditos_filho = creditos_filho + PERDE;
            }
            printf("Crédito: %d\n", creditos_filho);
        }
        write(fd2[ESCRITA], &TERMINA, BUFFER_SIZE);
        close(fd1[LEITURA]);
        close(fd2[ESCRITA]);
    }

    // Código do filho
    if (pid == 0) {
        srand(time(NULL) * getpid());
        close(fd1[LEITURA]);
        close(fd2[ESCRITA]);
        int continuar = 1;
        while (continuar) {
            read(fd2[LEITURA], &continuar, BUFFER_SIZE);
            //nr aleatório entre 1 e 5
            int nr_aleatorio = (rand() % 5) + 1;
            write(fd1[ESCRITA], &nr_aleatorio, BUFFER_SIZE);
        }
        close(fd1[ESCRITA]);
        close(fd2[LEITURA]);
        exit(EXIT_SUCCESS);
    }
    return 0;
}
