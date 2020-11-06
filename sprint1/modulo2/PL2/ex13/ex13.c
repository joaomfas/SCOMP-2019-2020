#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

int cria_filhos(int n) {
    pid_t pid;

    int i;
    for (i = 0; i < n; i++) {
        pid = fork();
        if (pid < 0) {
            return -1;
        } else if (pid == 0) {
            return i + 1;
        }
    }
    return 0;
}

int main(void) {
    int p12[2];
    int p23[2];
    int p34[2];
    int p4a[2];

    const int NUM_CHILDREN = 4;
    const int TOTAL_PIECES = 1000;
    int i = 0;
    int num_pcs = 0;
    int pcs_rec = 0;
    int pcs_rec_t = 0;

    enum extremidade {
        LEITURA = 0, ESCRITA = 1
    };

    if (pipe(p12) == -1 || pipe(p23) == -1 || pipe(p34) == -1 || pipe(p4a) == -1) {
        perror("Pipe failed!");
        exit(EXIT_FAILURE);
    }

    int id = cria_filhos(NUM_CHILDREN);
    if (id == -1) {
        perror("Fork failed!");
        exit(EXIT_FAILURE);
    }
    
    if (id == 0) { //Código do pai
        printf("armazem");
        //Pipes nao ligados a esta maquina
        close(p12[ESCRITA]);
        close(p23[ESCRITA]);
        close(p34[ESCRITA]);
        close(p12[LEITURA]);
        close(p23[LEITURA]);
        close(p34[LEITURA]);
        //

        //Pipes relacionados com esta maquina: fechar extremidades nao usadas
        close(p4a[ESCRITA]);
        //

        int inventory = 0;
        int n;
        int items = 0;
        while ((n = read(p4a[LEITURA], &items, sizeof (int))) > 0) {
            inventory += items;
            printf("Foram armazenados %d peças. Inventario = %d\n", items, inventory);
        }
        
        close(p4a[LEITURA]);

    } else { //Código dos filhos
        if (id == 1) { //Código para M1
            //Pipes nao ligados a esta maquina
            close(p23[ESCRITA]);
            close(p23[LEITURA]);
            close(p34[ESCRITA]);
            close(p34[LEITURA]);
            close(p4a[ESCRITA]);
            close(p4a[LEITURA]);
            //

            //Pipes relacionados com esta maquina: fechar extremidades nao usadas
            close(p12[LEITURA]);
            //
            
            for (i = 0; i < TOTAL_PIECES; i += 5) {
                num_pcs = 5;

                //Transfere as peças para M2
                if (write(p12[ESCRITA], &num_pcs, sizeof (int)) == -1) {
                    perror("Write failed!");
                }
                printf("M1: Enviei %d peças para M2\n", num_pcs);
            }
			close(p12[ESCRITA]);
        } else if (id == 2) { //Código para M2
            //Pipes nao relacionados com esta maquina
            close(p34[ESCRITA]);
            close(p34[LEITURA]);
            close(p4a[ESCRITA]);
            close(p4a[LEITURA]);
            //

            //Pipes relacionados com esta maquina: fechar extremidades nao usadas
            close(p12[ESCRITA]);
            close(p23[LEITURA]);
            //

            for (i = 0; i < TOTAL_PIECES; i += 5) {
                //Aguarda receber todas as peças para continuar
                while (pcs_rec_t < 5) {
                    //Recebe as peças de M1
                    if (read(p12[LEITURA], &pcs_rec, sizeof (int)) == -1) {
                        perror("Read failed!");
                        exit(EXIT_FAILURE);
                    }
                    pcs_rec_t += pcs_rec;
                }

                num_pcs = 5;

                //Transfere as peças para M3
                if (write(p23[ESCRITA], &num_pcs, sizeof (int)) == -1) {
                    perror("Write failed!");
                }
                printf("M2: Enviei %d peças para M3\n", num_pcs);
            }
			close(p12[LEITURA]);
            close(p23[ESCRITA]);
        } else if (id == 3) { //Código para M3
            //Pipes nao relacionados com esta maquina
            close(p12[ESCRITA]);
            close(p12[LEITURA]);
            close(p4a[ESCRITA]);
            close(p4a[LEITURA]);
            //

            //Pipes relacionados com esta maquina: fechar extremidades nao usadas
            close(p23[ESCRITA]);
            close(p34[LEITURA]);
            //

            for (i = 0; i < TOTAL_PIECES; i += 10) {
                //Aguarda receber todas as peças para continuar
                while (pcs_rec_t < 10) {
                    //Recebe as peças de M2
                    if (read(p23[LEITURA], &pcs_rec, sizeof (int)) == -1) {
                        perror("Read failed!");
                        exit(EXIT_FAILURE);
                    }
                    pcs_rec_t += pcs_rec;
                }

                num_pcs = 10;

                //Transfere as peças para M4
                if (write(p34[ESCRITA], &num_pcs, sizeof (int)) == -1) {
                    perror("Write failed!");
                }
				printf("M3: Enviei %d peças para M4\n", num_pcs);
            }
			close(p23[LEITURA]);
            close(p34[ESCRITA]);
        } else if (id == 4) { //Código para M4
            //Pipes nao relacionados com esta maquina
            close(p12[ESCRITA]);
            close(p12[LEITURA]);
            close(p23[ESCRITA]);
            close(p23[LEITURA]);
            //

            //Pipes relacionados com esta maquina: fechar extremidades nao usadas
            close(p34[ESCRITA]);
            close(p4a[LEITURA]);
            //

            for (i = 0; i < TOTAL_PIECES; i += 100) {
                //Aguarda receber todas as peças para continuar
                while (pcs_rec_t < 100) {
                    //Recebe as peças de M4
                    if (read(p34[LEITURA], &pcs_rec, sizeof (int)) == -1) {
                        perror("Read failed!");
                        exit(EXIT_FAILURE);
                    }
                    pcs_rec_t += pcs_rec;
                }

                num_pcs = 100;

                //Transfere as peças para o Armazem
                if (write(p4a[ESCRITA], &num_pcs, sizeof (int)) == -1) {
                    perror("Write failed!");
                }
                printf("M4: Enviei %d peças para o Armazem\n", num_pcs);
            }
            close(p34[LEITURA]);
            close(p4a[ESCRITA]);
        }

        exit(EXIT_SUCCESS);
    }

    //Pai espera que os filhos terminem
    int status;
    for (i = 0; i < NUM_CHILDREN; i++) {
        pid_t p = wait(&status);
        if (p == -1) {
            perror("Wait failed!");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
