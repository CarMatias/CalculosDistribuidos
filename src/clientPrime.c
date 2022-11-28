#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

int isPrime(int k)
{
	if (k <= 1)
		return 0;
	if (k == 2 || k == 3)
		return 1;

	if (k % 2 == 0 || k % 3 == 0)
		return 0;

	for (int i = 5; i * i <= k; i = i + 6)
		if (k % i == 0 || k % (i + 2) == 0)
			return 0;

	return 1;
}

int nThPrime(int n)
{
	int i = 2;
	while (n > 0)
	{
		if (isPrime(i))
			n--;
		i++;
	}
	i -= 1;
	return i;
}

int fib(int n)
{
	if (n <= 1)
		return n;
	return fib(n - 1) + fib(n - 2);
}

void handleConnection(int network_socket)
{
	int value, operation = 0;
	printf("Esperando a que el servidor envie un nuevo numero...\n");

	sleep(2);

	// Recibo el numero sobre el que tengo que realizar un calculo
	int len_recived = recv(network_socket, &value, sizeof(value), 0);
	if (len_recived < 0)
	{
		printf("Error al recibir el valor\n");
		exit(1);
	}
	else if (len_recived > 0)
	{
		// Recibo la operacion a realizar
		recv(network_socket, &operation, sizeof(operation), 0);
	}
	printf("El numero recibido del servidor es: %d\n", value);

	sleep(2);

	int nthPrime;

	// Realizo el calculo correspondiente
	switch (operation)
	{
	case 1:
		nthPrime = fib(value);
		break;
	case 2:
		nthPrime = nThPrime(value);
		break;
	default:
		printf("Operacion no valida\n");
		break;
	}

	// Envio el resultado al servidor
	send(network_socket, &nthPrime, sizeof(nthPrime), 0);
	printf("El numero enviado al servidor es: %d\n", nthPrime);

	sleep(2);

	close(network_socket);
	printf("Conexion cerrada\n");
}

void *clienthread(void *args)
{

	int client_request = *((int *)args);
	int network_socket;

	// Creo un socket de tipo TCP y stream
	network_socket = socket(AF_INET,
													SOCK_STREAM, 0);

	// Inicializo los puertos y direcciones
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(8989);

	// Realizo la conexion inicial con el servidor
	int connection_status = connect(network_socket,
																	(struct sockaddr *)&server_address,
																	sizeof(server_address));

	if (connection_status == -1)
	{
		printf("Ocurrio un error al realizar la conexion con el socket...\n");
		return 0;
	}

	// Llamo a la funcion que se encarga de
	// recibir los datos del server
	// y enviar los resultados
	handleConnection(network_socket);

	// Redefino el socket para realizar
	// una posible nueva conexion con el servidor
	network_socket = socket(AF_INET,
													SOCK_STREAM, 0);

	// Describo un while para que el cliente
	// pueda consultarle al server reiteradas veces
	// sin necesidad de cerrar y abrir el programa
	while (connect(network_socket,
								 (struct sockaddr *)&server_address,
								 sizeof(server_address)) >= 0)
	{
		handleConnection(network_socket);
	}
	pthread_exit(NULL);
	return 0;
}

int main()
{
	pthread_t tid;
	int client_request = 1;

	pthread_create(&tid, NULL,
								 clienthread,
								 &client_request);

	// Espero a que el thread termine
	pthread_join(tid, NULL);
}
