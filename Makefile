CC=gcc
CFLAGS=-Wall -g

13-1108912-1010208-10596proyecto2: 13-1108912-1010208-10596proyecto2.o estructuras.o
		$(CC) $(CFLAGS) -o 13-1108912-1010208-10596proyecto2 13-1108912-1010208-10596proyecto2.o estructuras.o

13-1108912-1010208-10596proyecto2.o: 13-1108912-1010208-10596proyecto2.c estructuras.h
		$(CC) $(CFLAGS) -pthread -c 13-1108912-1010208-10596proyecto2.c

estructuras.o: estructuras.c estructuras.h
		$(CC) $(CFLAGS) -c estructuras.c

clean:
	rm *.o 13-1108912-1010208-10596proyecto2