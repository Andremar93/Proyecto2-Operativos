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
                                Funciones
----------------------------------------------------------------------------*/

lista * nuevalista(char *llave);

contenedor * nuevo_contenedor(char* path);

unsigned int hash(char* str);

char * espchars(char str[]);