#pragma once


typedef enum estado_proceso {
    READY,
    RUNNING,
    WAITING,
    FINISHED
} Estado;
typedef struct proceso{
    char* nombre;
    int pid;
    int tiempo_inicio;
    Estado estado;
    int burst;
    int io_wait;
    double t_wait; //Esta variable (en segundos) se usa para saber en que momento se pauso el proceso
    char* path;
    char **argumentos;
    struct proceso* siguiente;
    int contador_uso_cpu; //Tiempo en estado running
    double turnaround_time;
    double response_time;
    double waiting_time;
    int codigo_exit;
} Proceso;


    




void ejecutar_proceso(Proceso* proceso);
void cambiar_estado(Proceso* proceso, Estado estado);
void obtener_estado(Proceso* proceso);
