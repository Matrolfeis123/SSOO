#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "process.h"

void cambiar_estado(Proceso* proceso, Estado estado){
    proceso->estado = estado;
}

void obtener_estado(Proceso* proceso){
    switch(proceso->estado){
        case READY:
            printf("Estado: READY\n");
            break;
        case RUNNING:
            printf("Estado: RUNNING\n");
            break;
        case WAITING:
            printf("Estado: WAITING\n");
            break;
        case FINISHED:
            printf("Estado: FINISHED\n");
            break;
    }
}

void ejecutar_proceso(Proceso* proceso){
    int inicio = clock(); //guarda el timepo de inicio del proceso
    int pid = fork();
    if (pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
        printf("Error al crear el proceso hijo\n");
    } else if(pid == 0){ // si estamos en proceso hijo:
        printf("Empieza proceso hijo\n");
        // Ejecutar proceso
        execv(proceso->path, proceso->argumentos); // No debiera ejecutarse nada de aca hacia abajo
        // Cambiar estado a FINISHED
        printf("Termina proceso hijo\n");
    } else{ //si estamos en proceso padre:
        printf("Empieza proceso padre\n");
        proceso->pid = pid;
        printf("pid del proceso hijo = %d\n", proceso->pid);
        }
    }

