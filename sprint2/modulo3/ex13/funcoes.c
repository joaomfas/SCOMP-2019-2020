#include "header.h"

char kWordParam[NUM_FILHOS][SEARCH_DESC_SIZE] = {
	 "Covid-19",
	 "crises",
	 "coronavírus",
	 "corona",
	 "vírus",
	 "OMS",
	 "saúde",
	 "humanos",
	 "diagnosticar",
	 "por"
};

void iniciaObjBusca(ObjBusca* objBusca){
    objBusca->flagInicio=0;
	objBusca->seqEscrita=0;

    const char * prefix = "arqs/arq_";
    const char * num = "00";
    const char * sufix = ".txt";
    
    int needed = strlen(prefix)+strlen(num)+strlen(sufix);

    for (int pos = 0; pos < NUM_FILHOS; pos++){
        char * fileName = (char *) malloc( sizeof( char ) * needed );
        char * posComp = (char *) malloc( sizeof( char ) * strlen(prefix));
        strcpy( fileName, prefix );    // store <- prefix
       if(pos<10){
            sprintf(posComp,"0%d",pos);
        }else{
            sprintf(posComp,"%d", pos);
        }
        strcat( fileName, posComp );
        strcat( fileName, sufix );
        strcpy(objBusca->filePath[pos], fileName);
        strcpy(objBusca->keyWord[pos], kWordParam[pos]);
        //strcpy(objBusca->filePath[pos], "arq_01.txt");
    }

}

int findkeyAndCount(char* keyWord, char* filePath){
    
    FILE *file;
    file = fopen(filePath, "r");

    if (file == NULL){
        printf("Não foi possível abrir o arq.\n");
        exit(EXIT_FAILURE);
    }
   
    int count = countOccurrences(file, keyWord);
    fclose(file);

    return count;
}


int countOccurrences(FILE *file, const char *keyWord){
    
    char str[SEARCH_DESC_SIZE];
    char *pos;
    int index, count; 
    count = 0;
    while ((fgets(str, SEARCH_DESC_SIZE, file)) != NULL){
        index = 0;
        while ((pos = strstr(str + index, keyWord)) != NULL){
            index = (pos - str) + 1;
            count++;
        }
    }
    return count;
}

