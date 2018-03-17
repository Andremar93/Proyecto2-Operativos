/*
Buscador-Indizador de archivos
Proyecto #2  Sistemas de Operacion CI3825

Autores:
            Andrea Martinez 12-10102
            Carlos Perez    13-11089
            Giuli Latella   08-10596
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<ftw.h>
#include<dirent.h>

/*----------------------------------------------------------------------------
                                Globales
----------------------------------------------------------------------------*/
char directorio_inicial[10000];
int altura_maxima = 20;
char archivo_indice[10000] = "indice.txt";
int update = 1;
int add = 1;

typedef struct contenedor{
    char palabra[1000];
    char direccion[10000];
}contenedor;


contenedor * nuevo_contenedor(char* str){
    contenedor *nuevo = (contenedor*) malloc(sizeof(contenedor));
    strcpy(nuevo->palabra, str);
    return nuevo;
}
/*----------------------------------------------------------------------------
                                Funciones
----------------------------------------------------------------------------*/

unsigned int hash(char* str) {
    unsigned int length = strlen(str);
    unsigned int hash = 5381;
    unsigned int i = 0;

    while(i < length){
        hash = ((hash << 5) + hash) + (*str);
        str++;
        i++;
    }

    return hash % 10000;
}

void add_hash(contenedor* agregar, contenedor* indice[]){
    unsigned int posicion = hash(agregar->palabra);
    if(indice[posicion] == NULL){
        indice[posicion] = agregar;
    }
    else{
        printf("Que haremos cuando hay colisiones?\n");
    }
}

/*----------------------------------------------------------------------------
                                    Main
----------------------------------------------------------------------------*/

int main(int argc, char *argv[]){
    getcwd(directorio_inicial, 10000);

    contenedor *indice[10000];

    //Leemos todos los flags correspondientes y cambiamos las variables 
    //globales para manejarlos
    int i = 0;
    printf("%d\n", argc);
    while(i < argc){
        if(strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--dir") == 0){
            i++;
            strcpy(directorio_inicial, argv[i]);
        }
        else if(strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--max") == 0){
            i++;
            altura_maxima = atoi(argv[i]);
        }
        else if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--index") == 0){
            i++;
            strcpy(archivo_indice, argv[i]);
        }
        else if(strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--noupdate") == 0){
            update = 0;
        }
        else if(strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--noadd") == 0){
            add = 0;
        }
        i++;
    }

    printf("i, argc: %d %d\n", i, argc);
    printf("update: %d\n", update);
    printf("add: %d\n", add);
    printf("altura maxima: %d\n", altura_maxima);
    printf("Directorio inicial: %s\n", directorio_inicial);
    printf("Archivo indice: %s\n", archivo_indice);

    
    contenedor *prueba = nuevo_contenedor("archivo.txt");

    add_hash(prueba, indice);
    add_hash(prueba, indice);

    prueba = nuevo_contenedor("quecosapasoaqui");

    add_hash(prueba, indice);

    printf("%s\n", indice[hash("archivo.txt")]->palabra);
    printf("%s\n", indice[hash("quecosapasoaqui")]->palabra);

    return EXIT_SUCCESS;
}