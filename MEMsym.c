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

unsigned int globalTime = 0;
unsigned int numFallos = 0;

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion( unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque);

typedef struct {
	unsigned char ETQ;
	unsigned char Data[TAM_LINEA];
} T_CACHE_LINE;

int main(){
	
	
	return 0;
}

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]){
	
}
void VolcarCACHE(T_CACHE_LINE *tbl){
	
}
void ParsearDireccion( unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque){
	
}
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque){
	
}
