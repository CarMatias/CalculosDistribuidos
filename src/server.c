#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

//Cantidad de Tareas
#define TASKS_SIZE 5	


//Estructura de comunicacion
struct send_socket_struct{
	int newSocket;
	int operation;
	long number;

} * send_value[TASKS_SIZE];


int tasks[TASKS_SIZE];

sem_t x, y;
pthread_t tid;
pthread_t writerthreads[100];
pthread_t readerthreads[100];
int readercount = 0;

//Carga de datos a operar
void fillTasks()
{
	int number, operation;
	for (int i = 0; i < TASKS_SIZE; i++)
	{
		printf("Ingrese el valor operar\n");
		scanf("%d", &number);
		send_value[i]->number = number;
		printf("Operacion\n1)Fibonacci\n2)Primo\n");
		scanf("%d", &operation);
		send_value[i]->operation = operation;
	}
}

//Verificacion de tareas completadas
int checkIfTasksAreDone()
{
	for (int i = 0; i < TASKS_SIZE; i++)
	{
		if (tasks[i] != -1)
		{
			return 0;
		}
	}
	return 1;
}

//Envio de tareas a los clientes conectados
void *enviarTarea(void *arg);


int main()
{
	int serverSocket, newSocket;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;

	socklen_t addr_size;

	//Asignacion de memoria a la estructura
	for (int i = 0; i < TASKS_SIZE; i++)
	{
			send_value[i] = malloc(sizeof(struct send_socket_struct) * 1);
	}
	

	fillTasks();

	//Config del socket y creacion
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8989);

	//Limpieza de puerto
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
		error("setsockopt(SO_REUSEADDR) failed");

	//Asignacion de direccion y puerto del socket
	if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{

		perror("Error en bind\n");
		exit(-1);
	}

	//Seteamos el socket para que pueda recibir conexiones
	if (listen(serverSocket, 50) == 0)
		printf("Esperando Conexión de clientes\n");
	else
		printf("Error\n");

	pthread_t tid[60];

	int i = 0;

	int done = 0;

	int result = 0;
	//Recibimos clientes hasta terminar todas las tareas
	while (done != 1)
	{
		addr_size = sizeof(serverStorage);
		//Aceptamos conexiones con los clientes
		newSocket = accept(serverSocket,(struct sockaddr *)&serverStorage,&addr_size);
		
		for (int i = 0; i < TASKS_SIZE; i++)
		{
			send_value[i]->newSocket = newSocket;
		}
		
		tasks[i] = -1;
		if (pthread_create(&tid[i], NULL, enviarTarea, send_value[i]) != 0)
			printf("Error al crear el Hilo");

		i++;
		done = checkIfTasksAreDone();
	}

	printf("Las tareas ya han sido enviadas!\n");
	for (int j = 0; j < TASKS_SIZE; j++)
	{
		int localRes = 0;
		pthread_join(tid[j], (void **)&localRes);
		result += localRes;
	}
	printf("Se recibieron los resultados\n");
	sleep(1);
	system("clear");
	printf("La sumatoria da como resultado: \n");

	printf("%d\n", result);

	//Cerramos el socket
	close(serverSocket);
	return 0;
}

void *enviarTarea(void *arguments)
{
	int socketDescriptor;
	struct send_socket_struct *send_value= arguments;
	int num = send_value->number;
	socketDescriptor = send_value->newSocket;
	int operation = send_value->operation;
	int result = 0;

	//Enviamos el valor a operar
	send(socketDescriptor, &num, sizeof(num), 0);
	//Enviamos la operacion a realizar
	send(socketDescriptor, &operation, sizeof(operation), 0);
	//Recibimos el resultado de la operacion enviada con su respectivo valor
	recv(socketDescriptor, &result, sizeof(result), 0);
	close(socketDescriptor);
	pthread_exit((void *)result);
}
