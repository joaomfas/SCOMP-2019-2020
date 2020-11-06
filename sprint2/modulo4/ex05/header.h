#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>

#define SEMS_NOME_BASE "/sem_ex5_"

#define NUM_FILHOS 1
#define MAX_TAM_STR 25
#define QTD_SEMS NUM_FILHOS


#endif
