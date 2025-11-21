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
#define RAM_FILE "CONTENTS_RAM.bin"
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
unsigned char simulRam[TAM_RAM];

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion( unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque);
int leeLineaFichero(FILE *fd, char *cadena);

int main(){
	T_CACHE_LINE tbl[8];
	FILE *fdirs = fopen(ADDR_FILE, "r");
	FILE *fbin = fopen(RAM_FILE, "rb");
	
	//comprobamos si se puede abrir el archivo
	if(fdirs == NULL){
		perror("Error al abrir el fichero");
		fclose(fbin);
		return(-1);
	}
	
	char linea[256];
	
	while(fgets(linea, sizeof(linea), fdirs)){
		printf("%s", linea);
	}
	
	//comprobamos si podemos abrir el fichero binario
	if(fbin == NULL){
		perror("Error al abrir el fichero");
		return(-1);
	}
	
	//leemos el fichero usando fread
	int leidos = fread(simulRam, 1, TAM_RAM, fbin);
	
	//Comprobamos si el tamaño del fichero es distinto al tamaño de la RAM 
	if(leidos != TAM_RAM){
		printf("Error de lectura");
		fclose(fbin);
		return(-1);
	}
	
	//cerramos ficheros
	fclose(fdirs);
	fclose(fbin);
	
	return(0);
}

//Con esta funcion hemos sido capaces de establacer todos los valores a los que empezo por defecto cuando la llamamos
void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]){
	for (int i = 0; i < NUM_FILAS; i++) {
        tbl[i].ETQ = ETQ_INICIAL;
        for (int j = 0; j < TAM_LINEA; j++) {
            tbl[i].Data[j] = DATA_INICIAL; 
        }
    }
}

//La funcion principal de esta funcion es la de mostrar los datos que tenemos en cache
void VolcarCACHE(T_CACHE_LINE *tbl){
	//Este primer bucle nos muestra las etiquetas en hexadecimal
	 for (int i = 0; i < NUM_FILAS; i++) {
        printf("%02X\n", tbl[i].ETQ);
    }
    //El primer bucle solo imprime la palabra datos en cada linea
    for (int i = 0; i < NUM_FILAS; i++) {
        printf("Datos:");
		//Este segundo bucle anidado nos imprime los datos en formato hexadecimal
        for (int j = TAM_LINEA - 1; j >= 0; j--) {
            printf("%02X ", tbl[i].Data[j]);
        }
        printf("\n");
    }
    printf("\n");
}
	


//ParsearDireccion funcina cogiendo un numero hexadecimal de 12 bits y lo divide en las 3 partes que necesita la cache, palabra, linea y etiqueta
void ParsearDireccion( unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque){
	*palabra = addr & 0x0F;
	
	*linea = (addr & 0x70) >> 4;
	
	*ETQ = (addr & 0xF80) >> 7;

	//Lo que hace el bloque es elimar los ultimos 4 bits o la parte de la palabra para asi poder saber exactamente el numero de bloque para cuando hay un fallo
	*bloque = addr >> 4;
	
}

//Esta funcion se ejecuta cuando la CPU pide un dato que no esta en cache y por tanto hay que ir a buscarlo a la RAM
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque){

    int j;
    int dir_inicial_ram;
	
    printf("Cargando el bloque %02X en la linea %02X\n", bloque, linea);

	//Sobreescribimos la antigua etiqueta por la nueva
    tbl[linea].ETQ = ETQ;

	//Calcula donde estan exactamente los datos en la RAM
    dir_inicial_ram = bloque * TAM_LINEA;

	//Este bucle sirve para cargar la linea entera en cache
    for (j = 0; j < TAM_LINEA; j++) {
        tbl[linea].Data[j] = MRAM[dir_inicial_ram + j];
    }
    

}
