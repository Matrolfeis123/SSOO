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

	struct proceso procesos[input_file->len];

	for (int i = 0; i < input_file->len; ++i)
	{
		procesos[i].nombre = input_file->lines[i][0];
		printf("%s \n", procesos[i].nombre);
		procesos[i].tiempo_inicio = atoi(input_file->lines[i][1]);
		printf("%d \n", procesos[i].tiempo_inicio);
		procesos[i].burst = atoi(input_file->lines[i][2]);
		printf("%d \n", procesos[i].burst);
		procesos[i].io_wait = atoi(input_file->lines[i][3]);
		printf("%d \n", procesos[i].io_wait);
		procesos[i].path = input_file->lines[i][4];
		printf("%s \n", procesos[i].path);
		
		for (int j = 0; j < 7; ++j)
		{
			printf("%s ", input_file->lines[i][j]);
		}
		printf("\n");
	}
	input_file_destroy(input_file);
}