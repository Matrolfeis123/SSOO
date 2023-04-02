#pragma once

typedef struct proceso{
    char nombre;
    int pid;
    int tiempo_inicio;
    char estado;
    int burst;
    int io_wait;
    char *path;
    char *argumentos; //Quizas son solo numeros revisar
    struct proceso* siguiente;
} Proceso;
