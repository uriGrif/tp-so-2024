#ifndef SOCKETS_H
#define SOCKETS_H

#include <sockets/sockets.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// SERVER

int socket_createTcpServer(char *host, char *port);
int socket_acceptConns(int server_fd);

// CLIENT
int socket_connectToServer(char *host, char *port);
void socket_freeConn(int socket_cliente);

int crear_conexion(char *ip, char* puerto);

#endif