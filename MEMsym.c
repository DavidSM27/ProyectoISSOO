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

int globalTime = 0;
int numFallos = 0;
int numAccesos = 0;
char simulRam[TAM_RAM];

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]);
void VolcarCACHE(T_CACHE_LINE *tbl);
void ParsearDireccion( unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque);
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ, int linea, int bloque);
int leeLineaFichero(FILE *fd, char *cadena);

int main(){
	T_CACHE_LINE tbl[8];
	FILE *fdirs = fopen(ADDR_FILE, "r");
	FILE *fbin = fopen(RAM_FILE, "rb");
	FILE *fcache_out;
	int addr;
	int etq;
	int palabra;
	int linea;
	int bloque;

	char texto[100];
	int indiceTexto = 0;
	char datoLeido;

	LimpiarCACHE(tbl);

	//comprobamos si podemos abrir el fichero binario
	if(fbin == NULL){
		perror("Error al abrir el fichero");
		return(-1);
	}

	//leemos el fichero usando fread
	size_t leidos = fread(simulRam, 1, TAM_RAM, fbin);

	//Comprobamos si el tamaño del fichero es distinto al tamaño de la RAM 
	if(leidos != TAM_RAM){
		printf("Error de lectura");
		fclose(fbin);
		return(-1);
	}

	//comprobamos si se puede abrir el archivo
	if(fdirs == NULL){
		perror("Error al abrir el fichero");
		fclose(fbin);
		return(-1);
	}

	while(fscanf(fdirs, "%x", &addr) == 1){
		//Aumentamos el numero de accesos totales
		numAccesos++;

		//Llamada a la funcion parsear direccion
		ParsearDireccion(addr, &etq, &palabra, &linea, &bloque);

		//Entra en el if si la etiqueta cargada coincide con la de la RAM
		if (tbl[linea].ETQ == etq)
		{
			//Aumentamos el tiempo de acceso total
			globalTime += 1;

			//Guardamos el dato leido para imprimirlo despues
			datoLeido = tbl[linea].Data[palabra];

			printf("T: %d, Acierto de CACHE, ADDR %04X Label %x linea %02X DATO %02X", globalTime, addr, etq, linea, datoLeido);
		}else{
			
			//Imprimimos el fallo
			printf("T: %d, Fallo de CACHE %d, ADDR %04X Label %X linea %02X palabra %02X bloque %02X", globalTime, numFallos, addr, etq, linea, palabra, bloque);

			//Llamamos a la funcion de tratarFallo
			TratarFallo(tbl, simulRam, etq, linea, bloque);

			//Guardamos el dato leido del fallo
			datoLeido = tbl[linea].Data[palabra];

		}
		
		//añadimos el dato leido a texto si  es in caracter imprimible
		 if (indiceTexto < 99) {
            if (datoLeido >= 32 && datoLeido <= 126) {
                texto[indiceTexto] = (char)datoLeido;
                indiceTexto++;
                texto[indiceTexto] = '\0'; 
            }
        }

        // Volcamos a la cache y hacemos el sleep
        VolcarCACHE(tbl);
        sleep(1);
		
	}

    // Mostramos las estadisticas
    printf("\n--- Estadisticas ---\n");
    float tiempo_medio;

	if (numAccesos > 0) {
		tiempo_medio = (float)globalTime / numAccesos;
	} else {
		tiempo_medio = 0.0;
	}

    printf("Accesos totales: %d; fallos: %d; Tiempo medio: %.2f\n", numAccesos, numFallos, tiempo_medio);
    printf("Texto leído: %s\n", texto);
	
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
	
	//Aumentamos el numero total de fallos
	numFallos++;
	//Aumentamos el tiempo total de acceso en 20
	globalTime += 20;
	
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
