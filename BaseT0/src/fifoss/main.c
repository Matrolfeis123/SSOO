#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <string.h> // strlen, strcpy, etc
#include <unistd.h> // fork, execv, etc
#include <sys/wait.h> // wait, waitpid, etc
#include <time.h> // clock, etc
#include "../process/process.h"
#include "../queue/queue.h"
#include "../file_manager/manager.h"


int main(int argc, char const *argv[])
{
	/*Lectura del input*/
	char *file_name = (char *)argv[1];
	InputFile *input_file = read_file(file_name);

	/*Mostramos el archivo de input en consola*/
	printf("Nombre archivo: %s\n", file_name);
	printf("Cantidad de procesos: %d\n", input_file->len);
	printf("Procesos:\n");
	for (int i = 0; i < input_file->len; ++i)
	{
		for (int j = 0; j < 7; ++j)
		{
			printf("%s ", input_file->lines[i][j]);
		}
		printf("\n");
	}


	/*Instanciar structs Procesos*/
	int cantidad_procesos = input_file->len;
	Proceso* arreglo_procesos = calloc(cantidad_procesos,sizeof(Proceso)); //Arreglo de Procesos en el heap



	for (int i = 0; i < cantidad_procesos; ++i)
	{
		Proceso proceso_i = {										// Instancio el proceso número 'i'
			.nombre = calloc(strlen(input_file->lines[i][0]) + 1, sizeof(char)), // Asignar memoria para el nombre
			.tiempo_inicio = atoi(input_file->lines[i][1]),
			.burst = atoi(input_file->lines[i][2]),
			.io_wait = atoi(input_file->lines[i][3]),
			.path = input_file->lines[i][4],							// .argumentos es un arreglo en heap de largo ARGc
			.argumentos = calloc(atoi(input_file->lines[i][5]),sizeof(char)), 
			.estado = READY,
		};
		strcpy(proceso_i.nombre, input_file->lines[i][0]); // Copiar el nombre del proceso. Recordar liberar memoria al final



		// Acá deberíamos printear el proceso para verificar correcto funcionamiento
		for (int j = 0; j < atoi(input_file->lines[i][5]); ++j)		//agrego los ARGi al arreglo vacío que había en .argumentos
		{
			proceso_i.argumentos[j] = input_file->lines[i][j+6];
		}

		arreglo_procesos[i] = proceso_i;			//apendo el proceso número 'i' al arreglo_procesos
	}
	input_file_destroy(input_file);

	/*Instanciar cola de procesos*/

	int min_idx;
	for (int i = 0; i < cantidad_procesos-1; ++i)
	{
		min_idx = i;
		for (int j = i+1; j < cantidad_procesos; ++j)
		{
			if (arreglo_procesos[j].tiempo_inicio < arreglo_procesos[min_idx].tiempo_inicio)
			{
				min_idx = j;
			}
		}
		Proceso temp = arreglo_procesos[min_idx];
		arreglo_procesos[min_idx] = arreglo_procesos[i];
		arreglo_procesos[i] = temp;
		
	}
	// Inicializar Cola
	Cola cola_procesos = {NULL, NULL};
	// Asigno puntero a proceso head y tail de cola
	cola_procesos.head = &arreglo_procesos[0];
	cola_procesos.tail = &arreglo_procesos[cantidad_procesos-1];


	//instanciar lista ligada
	for (int i = 0; i < cantidad_procesos; ++i)
	{
		//printf("tiempo inicio %s: %i\n", arreglo_procesos[i].nombre, arreglo_procesos[i].tiempo_inicio);
		if (i != cantidad_procesos-1)
		{
			arreglo_procesos[i].siguiente = &arreglo_procesos[i+1];
		}
		else
		{
			arreglo_procesos[i].siguiente = NULL;
		}
	}


	/*Ejecución de los procesos*/
	// Pasos:
	// 0. inicializar reloj y esperar a que el primer proceso llegue
	// 1. Ejecutar el proceso head de la cola
	// 2. Si el proceso terminó, sacarlo de la cola
	// 3. Si el proceso no terminó, moverlo al final de la cola
	// 4. Go to 1.

	// guardar variable de proceso actual:
	Proceso* proceso_actual = cola_procesos.head;


	while (cola_procesos.head != NULL )// FALTA esperar a que llegue el primer proceso. IDDEA: (&& tiempo_inicio >= proceso_actual->tiempo_inicio)
	{
		if (proceso_actual->estado == RUNNING){ //Si hay un proceso ejecutandose
			// Dentro de ejecutar proceso, se esta constantemente comprobando si el tiempo de cpu burst actual se termina, 
			// con lo que proceso_actual->estado se actualiza a WAITING o FINISHED
			ejecutar_proceso(proceso_actual); // Corroborar que es lo que entrega como output
		}
		else if (proceso_actual->estado == FINISHED){ //Si el proceso termino
			// sacarlo de la cola
			// liberar memoria
			// actualizar puntero head de la cola
			// actualizar puntero proceso_actual
		}
		else if (proceso_actual->estado == READY){ //Si el proceso no termino
			//Si es primera vez que el proceso se pone en ejecucion, cambiar a estado RUNNING
			//Si no es la primera vez, moverlo al final de la cola cola_procesos(?) o 
			
			// moverlo al final de la cola
			// actualizar puntero tail de la cola
			// actualizar puntero proceso_actual
		}	
		else if (proceso_actual->estado == WAITING){
			// Pasar a proceso a la cola de Waiting, hasta que se cumpla el tiempo de espera del proceso
			// al cumplirse el tiempo de espera, mover el proceso a la cola de Ready
			// actualizar puntero del proceso_actual (?)
		}



	int cpu_ocupada = 1;

	while (cola_procesos.head != NULL){ 									// Mientras hayan procesos en cola ready:
		if (cpu_ocupada == 0){												// Si cpu está desocupada
			cpu_ocupada = 1;												// la marco como ocupada
			ejecutar_proceso(cola_procesos.head);							// Ejecuto proceso head
			cpu_ocupada = 0;												// Marco cpu como desocupada
			if (&(*(cola_procesos.head)->siguiente) != NULL){				// Si hay un segundo proceso en cola ready,
				cola_procesos.head = &(*(cola_procesos.head)->siguiente);	// Actualizo el head al segundo proceso en cola ready
			}
		}
		//revisar_procesos_wait();				// COMPLETAR
	}
		






		// 0. inicializar reloj y esperar a que el primer proceso llegue
		double tiempo_cpu_burst_actual = proceso_actual->burst;
		clock_t tiempo_inicio_proceso = clock();
		clock_t cpu_burst_actual = clock() + tiempo_cpu_burst_actual * CLOCKS_PER_SEC; // tiempo del burst del proceso actual

		while(tiempo_inicio_proceso < cpu_burst_actual) // mientras el tiempo de cpu del proceso actual no se haya cumplido
		{
			ejecutar_proceso(proceso_actual); //creo que retorna el tiempo que demoro el proceso
		}

		// 1. Ejecutar el proceso head de la cola
		// 2. Si el proceso terminó, sacarlo de la cola
		// 3. Si el proceso no terminó, moverlo al final de la cola
		// 4. Go to 1.

	}

}


