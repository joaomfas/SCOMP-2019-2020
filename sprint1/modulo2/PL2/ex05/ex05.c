#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

int cria_filhos(int n){
	pid_t pid;
	
	int i;
	for(i = 0; i < n; i++){
		pid = fork();
		if(pid < 0){
			return -1;
		}else if(pid == 0){
			return i+1;
		}
	}
	return 0;
}

int main(void){
	const int MAX_SIZE = 256;
	char msg[MAX_SIZE];
	char msg_conv[MAX_SIZE];
	int up[2];
	int down[2];
	enum extremidade{LEITURA=0, ESCRITA=1};
	
	if(pipe(up) == -1 || pipe(down) == -1){
		perror("Pipe failed!");
		exit(EXIT_FAILURE);
	}
	
	int id = cria_filhos(1);
	if(id == -1){
		perror("Fork failed!");
		exit(EXIT_FAILURE);
	}
	
	if(id == 0){ //Código do pai
		close(up[ESCRITA]);
		close(down[LEITURA]);
		if(read(up[LEITURA], msg, sizeof(msg)) == -1){
			perror("Read failed!!");
			exit(EXIT_FAILURE);
		}
		close(up[LEITURA]);
				
		int i;
		for(i = 0; msg[i] != '\0'; i++){
			if(msg[i] >= 'a' && msg[i] <= 'z'){
				msg_conv[i] = msg[i] - 32;
			}else if(msg[i] >= 'A' && msg[i] <= 'Z'){
				msg_conv[i] = msg[i] + 32;
			}else{
				msg_conv[i] = msg[i];
			}
		}
		msg_conv[i] = '\0';
		
		if(write(down[ESCRITA], msg_conv, sizeof(msg_conv)) == -1){
			perror("Write failed!");
		}
		
		close(down[ESCRITA]);
	}else{ //Código do filho
		close(up[LEITURA]);
		close(down[ESCRITA]);
		printf("Insira uma mensagem: ");
		scanf("%s", msg);
		if(write(up[ESCRITA], msg, sizeof(msg)) == -1){
			perror("Write failed!");
		}
		close(up[ESCRITA]);
		printf("Mensagem original: %s\n", msg);
		
		printf("%d\n",sizeof(msg_conv));
		
		if(read(down[LEITURA], msg_conv, sizeof(msg_conv)) == -1){
			perror("Read failed!!");
			exit(EXIT_FAILURE);
		}
		close(down[LEITURA]);
		
		printf("Mensagem convertida: %s\n", msg_conv);
		
		exit(0);
	}
	
	int status;
	pid_t p = wait(&status);
	if(p == -1){
		perror("Wait failed!");
		exit(EXIT_FAILURE);
	}
	
	return 0;
}
