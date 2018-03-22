#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#include"estructuras.h"

/*----------------------------------------------------------------------------
                                Funciones
----------------------------------------------------------------------------*/

/*
Funcion nuevalista()

Parametros

    None

Funcion que crea una nueva estructura lista
*/

lista * nuevalista(char *llave){
    lista *nueva_lista = (lista*) malloc(sizeof(lista));
    strcpy(nueva_lista->llave, llave);
    nueva_lista->head = NULL;
    return nueva_lista;
}

/*
Funcion nuevo_contenedor(char* path)

Parametros

    path: char  ||  String que contiene la direccion del archivo encontrado.

Funcion que crea una nueva estructura contenedor, que contiene la direccion 
del archivo deseado
*/

contenedor * nuevo_contenedor(char* path){
    contenedor *nuevo = (contenedor*) malloc(sizeof(contenedor));
    strcpy(nuevo->direccion, path);
    nuevo->siguiente = NULL;
    return nuevo;
}

/*
Funcion hash(char* str)

Parametros

    str: char   ||  String que contiene la llave a la cual se le calculara
                    la posicion en la tabla de hash

Funcion que calcula la llave de hash para las palabras claves
*/

unsigned int hash(char* str) {
    unsigned int length = strlen(str);
    unsigned int hash = 5381;
    unsigned int i = 0;

    while(i < length){
        //Hacemos un bit shift left del numero para multiplicarlo por 33
        hash = ((hash << 5) + hash) + (*str);
        str++;
        i++;
    }

    return hash % 10000;
}

/*
Funcion espchars(char str[])

Parametros

    str: char   ||  String a la cual se le modificaran los caracteres con 
                    acento en mayuscula a minuscula

Funcion que recibe un string y tranforma a minuscula todos los caracteres
especiales que lo permitan
*/

char * espchars(char str[]){
    int i = 0;
    char especial[10000];
    while(i < strlen(str)){
        int j = 0;
        if(!isalpha(str[i])){
            while(i <= strlen(str) && j < 2){
                especial[j] = str[i];
                i++;
                j++;
            }
            if(strcmp(especial, "À") == 0){
                strcpy(especial, "à");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "Á") == 0){
                strcpy(especial, "á");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "Â") == 0){
                strcpy(especial, "â");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "Ã") == 0){
                strcpy(especial, "ã");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "Ä") == 0){
                strcpy(especial, "ä");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "Å") == 0){
                strcpy(especial, "å");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "Æ") == 0){
                strcpy(especial, "æ");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "Ç") == 0){
                strcpy(especial, "ç");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "È") == 0){
                strcpy(especial, "è");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "É") == 0){
                strcpy(especial, "é");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "Ê") == 0){
                strcpy(especial, "ê");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "Ë") == 0){
                strcpy(especial, "ë");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "Ì") == 0){
                strcpy(especial, "ì");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "Í") == 0){
                strcpy(especial, "í");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "Î") == 0){
                strcpy(especial, "î");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "Ï") == 0){
                strcpy(especial, "ï");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "Ð") == 0){
                strcpy(especial, "ð");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "Ñ") == 0){
                strcpy(especial, "ñ");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "Ò") == 0){
                strcpy(especial, "ò");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "Ó") == 0){
                strcpy(especial, "ó");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }
            if(strcmp(especial, "Ô") == 0){
                strcpy(especial, "ô");
                str[i-2] = especial[0];
                str[i-1] = especial[1];
            }

            i--;
        }
        else{
            i++;
        }
    }
    return str;
}