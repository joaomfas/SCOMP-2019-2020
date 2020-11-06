/**
Consider a supermarket with 5 barcode readers distributed along the shop. Every time a customer uses a barcode
reader the product’s name and its price should be printed on the screen. Simulate this system through processes
and pipes:
    a. The parent process has access to the products database;
    b. Each child process represents a barcode reader;
    c. There is a pipe shared by all 5 child processes to request the product’s information;
    d. The parent process replies to the requesting child with the corresponding product’s information through
    a pipe that it shares only with it. 
**/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NUM_FILHOS 30
#define QTD_PRODUTOS 3
#define NUM_LEITURAS 3

typedef struct{
    int codProduto;
    int pidCodeBarReader;
} ProducInfoRequest;

typedef struct{
    int codProduto;
    char desc[15];
    float preco;
} Produto;

void generateEstoqueDB(Produto[]);

const int LEITURA = 0;
const int ESCRITA = 1;

int main(int argc, char *argv[]){

    Produto estoqueDB[QTD_PRODUTOS];
    generateEstoqueDB(estoqueDB);

    pid_t generatedPid;
    int processSequenceNumber;
    int sharedRequestInfo[2];
    pipe(sharedRequestInfo);

    int sleepTimeForSync = 0;
   
    int adressTable[NUM_FILHOS][2];
   
    for(int a=0; a < NUM_FILHOS; a++){
        if (pipe(adressTable[a]) == -1) {
            perror("Pipe failed!");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_FILHOS; i++){
        processSequenceNumber = i;
        sleepTimeForSync += (1);
        generatedPid = fork();
        if (generatedPid == -1)
            return (1);
        if (generatedPid == 0)
            break;
    }

    if (generatedPid == 0){   

        close(sharedRequestInfo[LEITURA]);
        
        for(int a=0; a < NUM_FILHOS; a++){
            if(a != processSequenceNumber){
                close(adressTable[a][LEITURA]);
            }
             close(adressTable[a][ESCRITA]);
        }
    
        ProducInfoRequest prodRequest;
        prodRequest.pidCodeBarReader = processSequenceNumber;


        for (int i = 0; i < NUM_LEITURAS; i++){
        sleep(sleepTimeForSync);
            printf("\n");
            Produto produtoCB;
            prodRequest.codProduto = 100+i;
            write(sharedRequestInfo[ESCRITA], &prodRequest, sizeof(prodRequest));
            read(adressTable[processSequenceNumber][LEITURA], &produtoCB, sizeof(produtoCB));
            printf("%dª RESPOSTA - CB: %d - Informacoes produto -> Cod: %d Nome: %s Preco: %.2f \n", i+1,
             prodRequest.pidCodeBarReader, produtoCB.codProduto, produtoCB.desc, produtoCB.preco);
            printf("\n");
        }

        close(adressTable[processSequenceNumber][LEITURA]);
        exit(EXIT_SUCCESS);
    } 

    if (generatedPid > 0) {
        
        close(sharedRequestInfo[ESCRITA]);
        for(int a=0; a < NUM_FILHOS; a++){
            close(adressTable[a][LEITURA]);
        }
        
        for (int i = 0; i < (NUM_LEITURAS * NUM_FILHOS); i++){
            printf("\n");
            ProducInfoRequest prodRequest;
            read(sharedRequestInfo[LEITURA], &prodRequest, sizeof(prodRequest));
            printf("%dª requisicao BASE DE DADOS - Recebi um pedido de leitura de CB: %d -> Codigo requisitado: %d \n", i+1, prodRequest.pidCodeBarReader, prodRequest.codProduto);
            for (int j = 0; j < NUM_FILHOS; j++){
                if (j == prodRequest.pidCodeBarReader){
                    Produto produto;
                    for (int k = 0; k < QTD_PRODUTOS; k++){
                        if (estoqueDB[k].codProduto == prodRequest.codProduto){
                            produto.codProduto = estoqueDB[k].codProduto;
                            strcpy(produto.desc, estoqueDB[k].desc);
                            produto.preco = estoqueDB[k].preco;
                            break;
                        }
                    }
                    write(adressTable[j][ESCRITA], &produto, sizeof(produto));
                    break;
                }
            }
            printf("\n");
        }
        close(sharedRequestInfo[LEITURA]);
    }


    printf("\n\n\n=========================================================================\n");
    for (int i = 0; i < NUM_FILHOS; i++){
        close(adressTable[i][ESCRITA]);
        int ret1_pidChild, status;
        ret1_pidChild = wait(&status);
        if ((status >> 8) == -1){
            printf("\n>>Leitor CB %d RETORNOU COM ERRO %d \n", ret1_pidChild, (status >> 8));
        }else{
            printf("\nLeitor CB %d encerrou as solicitações. \n", ret1_pidChild);
        }
    }
    printf("\n\n=========================================================================\n");

    return 0;
}

void generateEstoqueDB(Produto list[]){
    for (int i = 0; i < QTD_PRODUTOS; i++) {
        if (i == 0){
            strcpy(list[i].desc, "banana");
            list[i].codProduto = 100+i;
            list[i].preco = 35.10;
        }else if (i == 1){
            strcpy(list[i].desc, "morango");
            list[i].codProduto = 100+i;
            list[i].preco = 23.89;
        }else if (i == 2){
            strcpy(list[i].desc, "goiaba");
            list[i].codProduto = 100+i;
            list[i].preco = 45.76;
        }
    }
}
