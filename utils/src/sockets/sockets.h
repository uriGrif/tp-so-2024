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
#include <pthread.h>
#include <proto/proto.h>

// SERVER
typedef void (*t_requestHandler)(uint8_t client_fd, uint8_t operation, t_buffer *buffer, void *args);

typedef struct socket_AsyncServerConf
{
    int **server_fds;
    int fds_size;
    int max_clients;
    /**
     * gets passed to all the handlers args (requestHandler, onConnectionClosed, onNewConnection).
     */
    void *handlers_args;
    /**
     * gets fired when a connected client sends data.
     */
    t_requestHandler requestHandler;
    /**
     * gets fired when client unexpectedly closes the connection
     */
    void (*onConnectionClosed)(int client_fd, void *args);
    /**
     * gets fired whenever a client makes the initial connection to the socket.
     */
    void (*onNewConnection)(int client_fd, void *args);
} socket_AsyncServerConf;

int socket_createTcpServer(char *host, char *port);
int socket_acceptConnsAsync(socket_AsyncServerConf *conf);
void server_listen(int fd, void (*connection_handler)(int *));

// CLIENT
int socket_connectToServer(char *host, char *port);
void socket_freeConn(int socket_cliente);

#endif