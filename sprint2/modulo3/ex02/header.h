#ifndef HEADER_H
#define HEADER_H

#define SHM_NAME "/shm_ex1"
#define MAX_DESC 20

typedef struct {
	int codigo;
	char desc[MAX_DESC];
	float preco;
	int novo;
} Produto;

#endif
