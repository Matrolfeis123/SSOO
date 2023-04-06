#pragma once
#include "../process/process.h"


typedef struct queue {
    Proceso *head;
    Proceso *tail;
}Cola;

void comprobar_waitings(Cola* cola_procesos, int tiempo_actual);