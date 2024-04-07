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
#include<pthread.h>

// SERVER

int socket_createTcpServer(char *host, char *port);
int socket_acceptConns(int server_fd);
void server_listen(int fd, void (*connection_handler)(int*));

// CLIENT
int socket_connectToServer(char *host, char *port);
void socket_freeConn(int socket_cliente);


#endif