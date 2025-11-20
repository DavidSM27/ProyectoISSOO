#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TAM_RAM 4096
#define TAM_LINEA 16
#define NUM_FILAS 8
#define TAM_CACHE 128
#define BITS_LINEA 3
#define BITS_PALABRA 4
#define BITS_ETQ 5
#define ETQ_INICIAL 0xFF
#define DATA_INICIAL 0x23
#define RAM_FILE "CONTENTS_RAM.txt"
#define ADDR_FILE "accesos_memoria.txt"
#define CACHE_OUT_FILE "CONTENTS_CACHE.bin"

#define MAX_LINEA 256

typedef struct {
	unsigned char ETQ;
	unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

unsigned int globalTime = 0;
unsigned int numFallos = 0;
unsigned int numAccesos = 0;

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion( unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque);
int leeLineaFichero(FILE *fd, char *cadena);

int main(){
	T_CACHE_LINE tbl[8];
	FILE *fd = fopen(ADDR_FILE, "r");
	
	char linea[MAX_LINEA];
	
	while(leeLineaFichero(fd, linea)){
		
		printf("Linea leida: %s\n", linea);
	}
	
	return 0;
}

int leeLineaFichero(FILE *fd, char *cadena) {
    int tam = 0;
    int c;

    while ((c = fgetc(fd)) != EOF && c != '\n') {
        if (tam < MAX_LINEA - 1) {   
            cadena[tam++] = (char)c;
        }
    }

    cadena[tam] = '\0'; 

    return (tam > 0 || c != EOF);
}

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]){
	for (int i = 0; i < NUM_FILAS; i++) {
        tbl[i].ETQ = ETQ_INICIAL;
        for (int j = 0; j < TAM_LINEA; j++) {
            tbl[i].Data[j] = DATA_INICIAL; 
        }
    }
}
/*void VolcarCACHE(T_CACHE_LINE *tbl){
	
	
}
void ParsearDireccion( unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque){
	
	
}
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque){
	
	
}*/
