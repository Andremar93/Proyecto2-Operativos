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
#include"estructuras.h"

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

lock: Variable que contiene el mutex lock para la exclusion mutua

tid: Variable que contendra la direccion del ultimo hilo iniciado

tid_maestro: variable que contiene la direccion del hilo que se encarga de
crear todos los demas hilos

directorios_visitados: tabla de hash que contiene las direcciones de los 
directorios que ya han sido visitados en el indice
*/

char directorio_inicial[10000];
int altura_maxima = 20;
char archivo_indice[10000] = "indice.txt";
int update = 1;
int add = 1;
lista *indice[10000];
pthread_mutex_t lock;
pthread_t tid; //Direcciones de los hilos
pthread_t tid_maestro;
contenedor *directorios_visitados[10000]; 

/*----------------------------------------------------------------------------
                                Funciones
----------------------------------------------------------------------------*/

/*
Funcion add_hash(contenedor* agregar)

Parametros

    arg: void   ||  El contenido del apuntador lo casteamos en un contenedor 
                    para sacar los datos del path a ingresar

Esta funcion se corre en un hilo por separado, en donde recibe un path a un 
archivo y lo parsea adecuandamente para verificarlo en la tabla de hash, en 
caso de no encontrarse en esta significa que no esta en el indice.
La funcion utiliza el mutex lock propuesto para los hilos para acceder a la 
tabla de hash.
*/

void * add_hash(void* arg){
    //Casteamos el contenedor del path a verificar
    contenedor *agregar = (contenedor *)arg;
    agregar->siguiente = NULL;

    //Extraemos la direccion al archivo dentro de la direccion
    int posicion_inicial = strlen(agregar->direccion) - 1;
    while(agregar->direccion[posicion_inicial] != '/' && posicion_inicial != -1){
        posicion_inicial--;
    }
    posicion_inicial++;

    //Extraemos el nombre del archivo para verificar si se encuentra dentro 
    //de la tabla
    int i = 0;
    char palabra[10000];
    while(posicion_inicial <= strlen(agregar->direccion)){
        palabra[i] = agregar->direccion[posicion_inicial];
        i++;
        posicion_inicial++;
    }

    //Extraemos una de las claves del nombre del archivo a ver si se encuentra 
    //en la tabla
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

        //transformamos la llave a minuscula puesto que no es case sensitive
        for(j = 0; j < strlen(llave); j++){
            llave[j] = tolower(llave[j]);
        }

        espchars(llave);

        //verificamos la tabla de hash
        unsigned int posicion = hash(llave);
        pthread_mutex_lock(&lock);
        while(indice[posicion] != NULL && posicion < 10000 && strcmp(llave, indice[posicion]->llave) != 0)
            posicion++;

        //En caso de no haber direcciones en esa llave, creamos una nueva 
        //lista y la agregamos a la tabla
        if(indice[posicion] == NULL){
            lista *nueva_llave = nuevalista(llave);
            contenedor *ingreso_tabla = nuevo_contenedor(agregar->direccion);
            nueva_llave->head = ingreso_tabla;
            indice[posicion] = nueva_llave;
        }
        //Si esta ocupada la posicion verificamos que la llave sea la misma
        else if(strcmp(llave, indice[posicion]->llave) == 0){
            //Agregar elemento a lista
            contenedor *lista_agregar = indice[posicion]->head;
            int direccion_ya_en_indice = 0;
            //Revisamos si la direccion exacta no se encuentra ya en la tabla
            while(lista_agregar->siguiente != NULL && direccion_ya_en_indice == 0){
                if(strcmp(lista_agregar->direccion, agregar->direccion) == 0){
                    direccion_ya_en_indice = 1;
                }
                lista_agregar = lista_agregar->siguiente;
            }
            //En caso de que no se encuentre la agregamos a la tabla
            if(direccion_ya_en_indice == 0 && strcmp(lista_agregar->direccion, agregar->direccion) != 0){
                contenedor *ingreso_tabla = nuevo_contenedor(agregar->direccion);
                lista_agregar->siguiente = ingreso_tabla;
            }
        }
        pthread_mutex_unlock(&lock);
    }

    return EXIT_SUCCESS;
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

Funcion que enviara los path de los archivos para ser ingresados en la tabla de
hash en caso de que no se encuentren ya en esta.
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
    if(((*inodo).st_mode & S_IFMT) == S_IFREG){
        //Si la direccion encontrada pertenece a un archivo, parseamos para 
        //sacar la informacion que nos interesa y verificamos si hay que 
        //agregarlo al indice y la tabla
        char direccion_parseo[10000];
        char direccion_archivo[10000];
        strcpy(direccion_parseo, nombre);
        strcpy(direccion_archivo, nombre);
        int i = 1;
        int altura = 0;
        while(i <= strlen(direccion_archivo)){
            if(direccion_archivo[i] == '/')
                altura++;
            i++;
        }

        if(altura < altura_maxima){
            int posicion_inicial = strlen(direccion_parseo) - 1;
            while(direccion_parseo[posicion_inicial] != '/' && posicion_inicial != 0){
                posicion_inicial--;
            }
            direccion_parseo[posicion_inicial] = '\0';
            //Verificamos si tenemos que actualizar la tabla y realizamos la 
            //accion correspondiente
            pthread_mutex_lock(&lock);
            if(update && directorios_visitados[hash(direccion_parseo)] != NULL){
                char nombre_archivo[10000];
                int i = 0;
                posicion_inicial++;
                while(posicion_inicial < strlen(direccion_archivo)){
                    nombre_archivo[i] = direccion_archivo[posicion_inicial];
                    posicion_inicial++;
                    i++;
                }
                nombre_archivo[i] = '\0';
                i = 0;
                while(i < strlen(nombre_archivo) && nombre_archivo[i] != '.' && nombre_archivo[i] != ' '){
                    i++;
                }
                nombre_archivo[i] = '\0';
                i = 0;
                while(i < strlen(nombre_archivo)){
                    nombre_archivo[i] = tolower(nombre_archivo[i]);
                    i++;
                }

                espchars(nombre_archivo);

                int posicion = hash(nombre_archivo);
                lista *revisar = indice[posicion];
                while(revisar != NULL && posicion < 10000 && strcmp(revisar->llave, nombre_archivo) != 0)
                    posicion++;
                int conseguido = 0;
                if(revisar != NULL){
                    contenedor *revisar_contenedor = revisar->head;
                    while(conseguido == 0 && revisar_contenedor != NULL){
                        if(strcmp(revisar_contenedor->direccion, direccion_archivo) == 0)
                            conseguido = 1;
                        revisar_contenedor = revisar_contenedor->siguiente;
                    }
                
                    if(conseguido == 0){
                        contenedor *agregar = nuevo_contenedor(direccion_archivo);
                        direccion_archivo[strlen(direccion_archivo)+1] = '\0';
                        direccion_archivo[strlen(direccion_archivo)] = '\n';
                        FILE *file = fopen(archivo_indice, "a+");
                        fputs(direccion_archivo, file);
                        fclose(file);
                        pthread_create(&(tid), NULL, add_hash, (void *)agregar);
                    }
                }
            }
            else if(add && directorios_visitados[hash(direccion_parseo)] == NULL){
                direccion_archivo[strlen(direccion_archivo)+1] = '\0';
                direccion_archivo[strlen(direccion_archivo)] = '\n';
                FILE *file = fopen(archivo_indice, "a");
                fputs(direccion_archivo, file);
                fclose(file);
                contenedor *agregar = nuevo_contenedor(direccion_archivo);
                pthread_create(&(tid), NULL, add_hash, (void *)agregar);
            }
            pthread_mutex_unlock(&lock);
        }
    }


    return 0;
}

/*
Funcion leer_archivo(vpod * arg)

Parametros

    arg: void   ||  Apuntador que reciben los procesos para hilos, el cual no
                    utilizamos

Funcion que llena la tabla de hash segun los datos en el archivo indice y se 
encarga de recorrer los directorios en busqueda de archivos que no se encuen
tren en la tabla para ser agregados a esta y al archivo.
*/

void * leer_archivo(void * arg){
    FILE *file = fopen(archivo_indice, "r");

    if(file != NULL){
        //Aqui se leen los paths en el indice y se parsean debidamente
        char direccion_indice[10000];

        while(fgets(direccion_indice, 10000, file) != NULL){
            direccion_indice[strlen(direccion_indice)-1] = '\0';

            //Llenamos la tabla de hash de palabras claves

            contenedor *llaves = nuevo_contenedor(direccion_indice);

            pthread_create(&(tid), NULL, add_hash, (void *)llaves);

            //Llenamos la tabla de hash de las direcciones visitadas

            int i = strlen(direccion_indice);
            while(i != 0 && direccion_indice[i] != '/')
                i--;

            direccion_indice[i] = '\0';

            contenedor *direccion_leida = nuevo_contenedor(direccion_indice);
            unsigned int llave = hash(direccion_indice);
            pthread_mutex_lock(&lock);
            if(directorios_visitados[llave] == NULL){
                directorios_visitados[llave] = direccion_leida;
            }
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
            pthread_mutex_unlock(&lock);
        }
        fclose(file);
    }

    if(add || update){
        ftw(directorio_inicial, &buscar_archivos, 1);
    }

    return EXIT_SUCCESS;

}


/*----------------------------------------------------------------------------
                                    Main
----------------------------------------------------------------------------*/

int main(int argc, char *argv[]){
    getcwd(directorio_inicial, 10000);

    //Leemos todos los flags correspondientes y cambiamos las variables 
    //globales para manejarlos
    if(argc <= 1){
        printf("Faltan argumentos\n");
        return EXIT_FAILURE;
    }
    if(argc > 10){
        printf("Demasiados argumentos\n");
        return EXIT_FAILURE;
    }
    int i = 0;
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
    char busqueda[10000];
    strcpy(busqueda, argv[argc-1]);

    i = 1;
    while(i < strlen(directorio_inicial)){
        if(directorio_inicial[i] == '/')
            altura_maxima++;
        i++;
    }

    if (pthread_mutex_init(&lock, NULL) != 0){
        printf("\n Fallo al inicializar el mutex lock \n");
        return EXIT_FAILURE;
    }

    int error = pthread_create(&(tid_maestro), NULL, leer_archivo, NULL);
    if (error != 0){
        printf("\nEl hilo no pudo ser creado:[%s]", strerror(error));
        return EXIT_FAILURE;
    }

    //Esperamos a que todos los hilos finalicen

    pthread_join(tid_maestro, NULL);
    pthread_join(tid, NULL);
    pthread_mutex_destroy(&lock);

    int posicion_busqueda = hash(busqueda);
    while(indice[posicion_busqueda] != NULL && posicion_busqueda < 10000 && strcmp(indice[posicion_busqueda]->llave, busqueda) != 0){
        posicion_busqueda++;
    }
    if(indice[posicion_busqueda] != NULL && strcmp(indice[posicion_busqueda]->llave, busqueda) == 0){
        contenedor* impresion = indice[posicion_busqueda]->head;
        while(impresion != NULL){
            printf("%s\n", impresion->direccion);
            impresion = impresion->siguiente;
        }
    }

    return EXIT_SUCCESS;
}