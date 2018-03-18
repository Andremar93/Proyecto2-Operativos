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
#include<pthread.h>

/*----------------------------------------------------------------------------
                                Estructuras
----------------------------------------------------------------------------*/

typedef struct contenedor{
    char direccion[10000];
    struct contenedor *siguiente;
}contenedor;

typedef struct lista{
    char llave[10000];
    contenedor *head;
}lista;

/*----------------------------------------------------------------------------
                                Globales
----------------------------------------------------------------------------*/
/*
directorio_inicial: Contiene la direccion del directorio donde se iniciara la 
busqueda para la indizacion

altura_maxima: contiene la altura maxima de profundidad a la hora de buscar 
nuevos archivos para su introduccion en el indice

archivo_indice: Contiene el nombre del archivo que contiene el indice previa-
mente establecido para su devolucion en busquedas

update: Variable que indica que no se debe entrar en los directorio que ya 
estan en el indice

add: Indica que no se debe entrar en los directorios que no esten en el indice

indice: arreglo que contiene las direcciones de los objetos que contienen la
informacion de los archivos pertenecientes al indice.
*/

char directorio_inicial[10000];
int altura_maxima = 20;
char archivo_indice[10000] = "indice.txt";
int update = 1;
int add = 1;
lista *indice[10000];
pthread_mutex_t lock;
pthread_t tid[3]; //Direcciones de los hilos

//La lista siguiente pienso utilizarla para llevar cuenta de los directorios visitados
//y asi poder trabajar con add y update
contenedor *directorios_visitados[10000]; 

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

/*
Funcion add_hash(contenedor* agregar, lista* indice[])

Parametros

    None

Aun no esta lista
*/

void add_hash(contenedor* agregar, lista* indice[]){
    agregar->siguiente = NULL;
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


        for(j = 0; j < strlen(llave); j++){
            llave[j] = tolower(llave[j]);
        }

        espchars(llave);

        printf("---------------------------\n");

        unsigned int posicion = hash(llave);

        printf("llave: %s\n", llave);
        printf("posicion: %d\n", posicion);

        if(indice[posicion] == NULL){
            lista *nueva_llave = nuevalista(llave);
            nueva_llave->head = agregar;
            indice[posicion] = nueva_llave;
        }
        else{
            //Agregar elemento a lista

            //AQUI HAY QUE USAR LA PALABRA CLAVE EN LA LISTA PARA VER SI TENEMOS QUE HACER REHASH O NO
            contenedor *lista_agregar = indice[posicion]->head;
            printf("AQUI ESTO ES DEBIGGING%s\n", indice[posicion]->llave);
            int direccion_ya_en_indice = 0;
            while(lista_agregar->siguiente != NULL && direccion_ya_en_indice == 0){
                if(strcmp(lista_agregar->direccion, agregar->direccion) == 0){
                    direccion_ya_en_indice = 1;
                }
                lista_agregar = lista_agregar->siguiente;
            }
            if(direccion_ya_en_indice == 0 && strcmp(lista_agregar->direccion, agregar->direccion) != 0){
                printf("Agregada direccion %s\n", agregar->direccion);
                lista_agregar->siguiente = agregar;
            }
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
        printf("Se encontro un archivo: %s\n", nombre);

    return 0;
}

int prueba = 0;

void * leer_archivo(void * arg){
    pthread_mutex_lock(&lock);

    //Leer archivo aqui

    FILE *file = fopen(archivo_indice, "r");

    if(file != NULL){
        //Aqui se leen los paths en el indice y se parsean debidamente
        char direccion_indice[10000];

        while(fgets(direccion_indice, 10000, file) != NULL){
            direccion_indice[strlen(direccion_indice)-1] = '\0';
            printf("%s\n", direccion_indice);

            //Llenamos la tabla de hash de palabras claves

            contenedor *llaves = nuevo_contenedor(direccion_indice);

            printf("La llave tiene la siguiente direccion: %s\n", llaves->direccion);

            add_hash(llaves, indice);

            //Llenamos la tabla de hash de las direcciones visitadas

            int i = strlen(direccion_indice);
            while(i != 0 && direccion_indice[i] != '/')
                i--;

            direccion_indice[i] = '\0';
            printf("%s\n", direccion_indice);

            contenedor *direccion_leida = nuevo_contenedor(direccion_indice);
            unsigned int llave = hash(direccion_indice);
            if(directorios_visitados[llave] == NULL)
                directorios_visitados[llave] = direccion_leida;
            else{
                contenedor *busqueda = directorios_visitados[llave];
                int encontrado = 0;

                while(busqueda->siguiente != NULL && encontrado == 0){
                    if(strcmp(busqueda->direccion, direccion_leida->direccion) == 0)
                        encontrado = 1;
                    busqueda = busqueda->siguiente;
                }

                if(encontrado == 0 && strcmp(busqueda->direccion, direccion_leida->direccion) != 0)
                        busqueda->siguiente = direccion_leida;
            }

            

        }
    }
    else{
        printf("El archivo %s no existe\n", archivo_indice);
        if(add){
            ftw(directorio_inicial, &buscar_archivos, 1);
        }
    }

    pthread_mutex_unlock(&lock);

    return EXIT_SUCCESS;

}


/*----------------------------------------------------------------------------
                                    Main
----------------------------------------------------------------------------*/

int main(int argc, char *argv[]){
    getcwd(directorio_inicial, 10000);

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


    //Como llamar el ftw
    //ftw(directorio_inicial, &buscar_archivos, 1);

    //printf("%s\n", indice[hash("caminote")]->head->direccion);
    //printf("%s\n", indice[hash("de")]->head->direccion);

    if (pthread_mutex_init(&lock, NULL) != 0){
        printf("\n Fallo al inicializar el mutex lock \n");
        return EXIT_FAILURE;
    }
    /*
    i = 0;
    int error;
    while(i < 2){
        error = pthread_create(&(tid[i]), NULL, &leer_archivo, NULL);
        if (error != 0)
            printf("\nEl hilo no pudo ser creado:[%s]", strerror(error));
        i++;
    }
    */

    int error = pthread_create(&(tid[0]), NULL, &leer_archivo, NULL);
    if (error != 0)
        printf("\nEl hilo no pudo ser creado:[%s]", strerror(error));

    //Esperamos a que todos los hilos finalicen

    pthread_join(tid[0], NULL);
    pthread_mutex_destroy(&lock);

    contenedor *prueba = nuevo_contenedor("/caminito de maiz.painting");

    add_hash(prueba, indice);

    contenedor *prueba2 = nuevo_contenedor("/caminote de perejil");

    add_hash(prueba2, indice);

    printf("%s\n", indice[hash("creeme")]->head->direccion);
    printf("%s\n", indice[hash("caminito")]->head->siguiente->siguiente->direccion);
    

    return EXIT_SUCCESS;
}

/*
Pruebas de parametros de entrada
printf("i, argc: %d %d\n", i, argc);
    printf("update: %d\n", update);
    printf("add: %d\n", add);
    printf("altura maxima: %d\n", altura_maxima);
    printf("Directorio inicial: %s\n", directorio_inicial);
    printf("Archivo indice: %s\n", archivo_indice);
*/