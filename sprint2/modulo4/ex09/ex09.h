#ifndef EX09_H
#define EX09_H

#define SHM_NAME "/shm_ex9"
#define SEM_NAME "/sema_ex9"
#define NUM_CHILDREN 1
#define MAX_TAM_STR 25

typedef struct {
    int beer;
    int chips;
} Fridge;

void buy_chips(Fridge* f);
void eat_and_drink(Fridge* f);

#endif
