#pragma once


typedef enum estado_proceso {
    READY,
    RUNNING,
    WAITING,
    FINISHED
} Estado;


typedef struct proceso{
    char nombre;
    int pid;
    int tiempo_inicio;
    Estado estado;
    int burst;
    int io_wait;
    char *path;
    char *argumentos;
    struct proceso* siguiente;
} Proceso;
