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
        // Cambiar estado a RUNNING
        cambiar_estado(proceso, RUNNING);
        obtener_estado(proceso);
        // Esperar a que finalice proceso hijo o esperar señal de IO
        while(proceso->estado != FINISHED && proceso->estado != WAITING){
            // definir tiempo_transcurrido:
            int tiempo_transcurrido = (clock() - inicio)/CLOCKS_PER_SEC;
            // si tiempo_transcurrido >= burst:
            if(tiempo_transcurrido >= proceso->burst){
                // Si el proceso termina antes de tiempo, pausar y moverlo a la cola de waiting
                //1. Enviar señal de pausa a proceso hijo con signal
                signal(SIGSTOP, proceso->pid);
                cambiar_estado(proceso, WAITING);
                obtener_estado(proceso);
                break; //CORROBORAR SI ESTE BREAK HACE Q SE VUELVA A CORRER EL WHILE
                //Falta implementar cola de WAITINGS
            }
            //Si se pasa de este if, el proceso logro terminar

////////////////////REVISAR////////////////////////////////////////////REVISAR////////////////////////
            // En esta parte, si pasa hasta aca el proceso termina, y se debe cambiar el estado a FINISHED
            
            int pid_hijo = waitpid(proceso->pid, NULL, WNOHANG); //No estoy seguro si esta bien utilizado WNOHANG
            if (pid_hijo == -1){
                //error
                printf("Error al esperar al proceso hijo\n");
                exit(EXIT_FAILURE);
            }
            if (pid_hijo == 0){
                //proceso hijo aun no termina
                continue;
            }
            else{
                //proceso hijo termina
                cambiar_estado(proceso, FINISHED);
                obtener_estado(proceso);
                break;
                // Esto gatilla la ruptura del if que se esta ejecutanado. Posibles problemas: tengo un while(cola_procesos != NULL) en el main, y si el proceso termina, se podria romper el while, pero no debiera.
            }
////////////////////REVISAR////////////////////////////////////////////REVISAR////////////////////////
        }
    }
}