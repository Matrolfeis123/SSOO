#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <string.h> // strlen, strcpy, etc
#include <unistd.h> // fork, execv, etc
#include <sys/wait.h> // wait, waitpid, etc
#include <time.h> // clock, etc
#include <signal.h> // signal, etc
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
			.t_wait = 0, // Momento en que inicia su I/O_Burst

		};
		strcpy(proceso_i.nombre, input_file->lines[i][0]); // Copiar el nombre del proceso. Recordar liberar memoria al final



		// Acá deberíamos printear el proceso para verificar correcto funcionamiento

		for (int j = 0; j < atoi(input_file->lines[i][5]); ++j)		//agrego los ARGi al arreglo vacío que había en .argumentos
		{
			proceso_i.argumentos[j] = input_file->lines[i][j+5];
		}

		arreglo_procesos[i] = proceso_i;			//apendo el proceso número 'i' al arreglo_procesos
	}
	input_file_destroy(input_file);


	/*Instanciar cola de procesos*/

	// Pasos:
	// 1. Encontrar el menor tiempo de inicio si el proceso no es NULL
	// 2. Agregarlo a la cola
	// 3. Hago su valor NULL en el arreglo_procesos
	// 4. Go to 1.

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



	//imprimir cola de procesos
	for (int i = 0; i < cantidad_procesos; ++i)
	{
		printf("tiempo inicio %s: %i\n", arreglo_procesos[i].nombre, arreglo_procesos[i].tiempo_inicio);

		if (i < cantidad_procesos-1)
		{
			arreglo_procesos[i].siguiente = &arreglo_procesos[i+1];
			printf("proceso nombre: %s\n", arreglo_procesos[i].siguiente->nombre);
		}
		else
		{
			arreglo_procesos[i].siguiente = NULL;
			printf("proceso nombre: %p\n", arreglo_procesos[i].siguiente);
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
	


	// Inicializar reloj
	int inicio = clock()/CLOCKS_PER_SEC;
	while (cola_procesos.head != NULL)// FALTA esperar a que llegue el primer proceso. IDDEA: (&& tiempo_inicio >= proceso_actual->tiempo_inicio)
	{	Proceso* proceso_actual = cola_procesos.head;

		if (proceso_actual->estado == RUNNING){ //Si hay un proceso ejecutandose
			// Dentro de ejecutar proceso, se esta constantemente comprobando si el tiempo de cpu burst actual se termina, 
			// con lo que proceso_actual->estado se actualiza a WAITING o FINISHED
			
			if (proceso_actual->t_wait == 0) // Si es la primera vez que se ejecuta el proceso, ejecutar 
			{	
				// Imprimir comienza ejecucion proceso
				printf("Comienza primera ejecución de %s\n", proceso_actual->nombre);
				ejecutar_proceso(proceso_actual); // Corroborar que es lo que entrega como output

			} else {
				// Si no es la primera vez que se ejecuta el proceso, enviar señal de continuar
				printf("Se envia señal de continuar a %s, pid = %i\n", proceso_actual->nombre, proceso_actual->pid);
				kill(proceso_actual->pid, SIGCONT);
			}

			// Esperar a que finalice proceso hijo o esperar señal de IO
			while(proceso_actual->estado != FINISHED && proceso_actual->estado != WAITING){
				
				// definir tiempo_transcurrido:
				int tiempo_transcurrido = (clock() - inicio)/CLOCKS_PER_SEC;
				// si tiempo_transcurrido >= burst:
				if(tiempo_transcurrido >= proceso_actual->burst){
					// Si el proceso termina antes de tiempo, pausar y moverlo a la cola de waiting
					//1. Enviar señal de pausa a proceso hijo con signal
					printf("Se envia señal de pausa a %s, pid = %i\n", proceso_actual->nombre, proceso_actual->pid);
					proceso_actual->t_wait = clock()/CLOCKS_PER_SEC;
					proceso_actual->estado = WAITING;
					kill(SIGSTOP, proceso_actual->pid); //Si esque hay error, entonces usar comando kill()
					
					//Guardar tiempo en cuanto inicia espera
					break; //CORROBORAR SI ESTE BREAK HACE Q SE VUELVA A CORRER EL WHILE
					// Ademas, nose si debo comprobar si el proceso termino o no
				}
				int status;
				int codigo_wait = waitpid(proceso_actual->pid, &status, WNOHANG);
		
				// if (waitpid(proceso_actual->pid, &status, WNOHANG) == -1){
				// //error
				// printf("Error al esperar al proceso_actual hijo\n");
				// exit(EXIT_FAILURE);
				// }
				
				if (WIFEXITED(status)){
					//proceso_actual hijo termina
					//Imprimir que se recibio señal de termino de proceso hijo
					printf("Se recibio señal de termino proceso hijo pid = %i\n", proceso_actual->pid);
					cambiar_estado(proceso_actual, FINISHED);
					obtener_estado(proceso_actual);
					break;
				}
				else if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT){
					//proceso hijo termina por una señal de interrupcion
					printf("Se recibio señal de interrupcion proceso hijo pid = %i\n", proceso_actual->pid);
					cambiar_estado(proceso_actual, FINISHED);
					obtener_estado(proceso_actual);
					break;
				}

				else if (WIFCONTINUED(status)){
					//proceso hijo continua
					printf("Se recibio señal de continuar proceso hijo pid = %i\n", proceso_actual->pid);
					cambiar_estado(proceso_actual, RUNNING);
					obtener_estado(proceso_actual);
					break;
				}
				else if (WIFSTOPPED(status)){
    				//proceso hijo es detenido por una señal de pausa
					printf("Se recibio señal de pausa proceso hijo pid = %i\n", proceso_actual->pid);
    				cambiar_estado(proceso_actual, WAITING);
    				obtener_estado(proceso_actual);
    				break;
				}
			}
		}

		else if (proceso_actual->estado == FINISHED){ //Si el proceso termino
			// Sacar proceso de la cola		
			printf("Se saca de la cola a %s\n", proceso_actual->nombre);
			cola_procesos.head = &(*(cola_procesos.head)->siguiente);
		}

		else if (proceso_actual->estado == READY){
			// Comprobar si se cumplio su tiempo de inicio
			// Si se cumplio, cambiar estado a running
			// actualizar puntero proceso_actual
			while(clock()/CLOCKS_PER_SEC - inicio < proceso_actual->tiempo_inicio){
				//Esperar a que se cumpla el tiempo de inicio
				continue;
			}
			proceso_actual->estado = RUNNING;
			
		}	
		
		else if (proceso_actual->estado == WAITING){
			// Comprobar si ya se cumplio su tiempo io_wait
			// Si se cumplio, moverlo a la cola de Ready
			// actualizar puntero proceso_actual
			
			if (clock()/CLOCKS_PER_SEC - proceso_actual->t_wait >= proceso_actual->io_wait){
					proceso_actual->estado = RUNNING;
					cola_procesos.head = proceso_actual;
				}
			else{ //Enviar proceso al ultimo de la cola
				Proceso* aux = cola_procesos.head;
				while(aux->siguiente != NULL){
					aux = aux->siguiente;
				}
				aux->siguiente = proceso_actual;
				proceso_actual->siguiente = NULL;
				

			}
		// AQUI COMPROBAR EL ESTADO DE LOS PROCESOS WAITING Y VER SI SU TIEMPO DE ESPERA YA SE CUMPLIO, para pasarlo a ready
			// al cumplirse el tiempo de espera, mover el proceso a la cola de Ready
			// actualizar puntero del proceso_actual (?)

		}
}


