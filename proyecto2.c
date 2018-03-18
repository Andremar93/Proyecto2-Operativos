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
    char direccion[10000];
    struct contenedor *siguiente;
}contenedor;

typedef struct lista{
    char llave[10000];
    contenedor *head;
}lista;

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

contenedor * nuevo_contenedor(char* path){
    contenedor *nuevo = (contenedor*) malloc(sizeof(contenedor));
    strcpy(nuevo->direccion, path);
    nuevo->siguiente = NULL;
    return nuevo;
}

void insertarContenedor(lista *presentes, contenedor *insertar){

}

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

void add_hash(contenedor* agregar, lista* indice[]){

    int posicion_inicial = strlen(agregar->direccion) - 1;
    while(agregar->direccion[posicion_inicial] != '/' || posicion_inicial == -1){
        posicion_inicial--;
    }
    posicion_inicial++;

    int i = 0;
    char palabra[10000];
    while(posicion_inicial <= strlen(agregar->direccion)){
        palabra[i] = agregar->direccion[posicion_inicial];
        i++;
        posicion_inicial++;
    }

    printf("%s\n", agregar->direccion);

    i = 0;
    while(i < strlen(palabra)){
        char llave[10000];
        int j = 0; 
        while(i < strlen(palabra) && palabra[i] != ' ' && palabra[i] != '.'){
            llave[j] = palabra[i];
            i++;
            j++;
        }
        llave[j] = '\0';
        i++;

        printf("%s\n", llave);

        unsigned int posicion = hash(llave);

        if(indice[posicion] == NULL){
            lista *nueva_llave = nuevalista(llave);
            nueva_llave->head = agregar;
            indice[posicion] = nueva_llave;
        }
        else{
            //Agregar elemento a lista

            //AQUI HAY QUE USAR LA PALABRA CLAVE EN LA LISTA PARA VER SI TENEMOS QUE HACER REHASH O NO
            contenedor *lista_agregar = indice[posicion]->head;
            while(lista_agregar->siguiente != NULL){
                lista_agregar = lista_agregar->siguiente;
            }
            lista_agregar->siguiente = agregar;
        }
    }
}

/*
Funcion buscar_directorios(const char *name, const struct stat *inodo, int type)

Parametros

    nombre: const char            ||  String que contiene el path del directorio
                                    actual
    inodo: const struct stat    ||  Estructura stat que representa el inodo
                                    del directorio o archivo actual
    tipo: int                   ||  Entero que representa el tipo de elemento
                                    actual

Funcion que enviara los directorios hoja al proceso de lowercase para ser 
parseados y verificados
*/

/*
    INFORMACION DE INTERES

    S_IFMT     0170000   bit mask for the file type bit field

    S_IFSOCK   0140000   socket
    S_IFLNK    0120000   symbolic link
    S_IFREG    0100000   regular file
    S_IFBLK    0060000   block device
    S_IFDIR    0040000   directory
    S_IFCHR    0020000   character device
    S_IFIFO    0010000   FIFO
*/

int buscar_archivos(const char *nombre, const struct stat *inodo, int tipo){
    //Verificamos si el path actual corresponde a un archivo solo si esta el 
    //flag de archivos prendido
    if(((*inodo).st_mode & S_IFMT) == S_IFREG)
        printf("Se encontro un archivo\n");

    return 0;
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

/*----------------------------------------------------------------------------
                                    Main
----------------------------------------------------------------------------*/

int main(int argc, char *argv[]){
    getcwd(directorio_inicial, 10000);

    lista *indice[10000];

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

    
    contenedor *prueba = nuevo_contenedor("/caminito de maiz.painting");

    add_hash(prueba, indice);

    prueba = nuevo_contenedor("/caminote de perejil");

    add_hash(prueba, indice);

    printf("%s\n", indice[hash("caminote")]->head->direccion);
    printf("%s\n", indice[hash("maiz")]->head->direccion);

    return EXIT_SUCCESS;
}

//Agregarle la palabra clave a la lista

