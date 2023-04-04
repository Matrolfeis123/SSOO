#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
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
			.nombre = input_file->lines[i][0],						// y le agrego sus atributos
			.tiempo_inicio = atoi(input_file->lines[i][1]),
			.burst = atoi(input_file->lines[i][2]),
			.io_wait = atoi(input_file->lines[i][3]),
			.path = input_file->lines[i][4],							// .argumentos es un arreglo en heap de largo ARGc
			.argumentos = calloc(atoi(input_file->lines[i][5]),sizeof(char)), 
			.estado = READY,
		};


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

	Proceso* dir_principio_cola;  // Falta asignar!
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
	//imprimir cola de procesos
	printf("%i\n", cantidad_procesos);
	for (int i = 0; i < cantidad_procesos; ++i)
	{
		printf("tiempo inicio %s: %i\n", arreglo_procesos[i].nombre, arreglo_procesos[i].tiempo_inicio);
		if (i != cantidad_procesos-1)
		{
			arreglo_procesos[i].siguiente = &arreglo_procesos[i+1];
		}
		else
		{
			arreglo_procesos[i].siguiente = NULL;
		}
	}
}

//agustin volvio a editar
