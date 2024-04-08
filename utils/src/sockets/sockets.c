#include <sockets/sockets.h>
#include <proto/proto.h>
#include <sys/epoll.h>
#include <errno.h>
#include "fcntl.h"

enum SocketType
{
    SERVER,
    CLIENT
};

/**
 * private fns
 */
static int setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        return -1;
    return 0;
}

/**
 *
 * @param host set is a NULL if you wish it to be your computer host.
 * @returns a linked list with several hosts for us to create a socket and bind/connect the socket to or `NULL` on error.
 */
static struct addrinfo *getAddrInfo(char *host, char *port, enum SocketType type)
{
    // restrictions imposed to getaddrinfo
    struct addrinfo hints;
    struct addrinfo *addrInfo;
    memset(&hints, 0, sizeof(hints));
    // don't specify any specific address, so that the socket is accesible through multiple protocols (IPV4, IPV6)
    hints.ai_family = AF_UNSPEC;
    // tcp streams
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    // specify this flag only on server sockets, so that the returned lists includes sockets suitable for binding and accepting connections
    if (type == SERVER)
        hints.ai_flags = AI_PASSIVE;
    hints.ai_canonname = NULL;

    int status = getaddrinfo(host, port, &hints, &addrInfo);
    if (status != 0)
        return NULL;

    return addrInfo;
}

/**
 * lib fns (must be prefixed with socket_)
 */

/**
 * @param requestHandler pass NULL if you don't expect a response from the server.
 * @returns `-1` when client is closed
 */
int socket_read(int fd, t_requestHandler requestHandler, void *args)
{
    t_packet *packet = packet_new(-1);
    if (packet == NULL)
        return 0;
    // op code and buffer size must always be explicit in the messages
    int bytes_read;
    bytes_read = recv(fd, &(packet->op_code), sizeof(uint8_t), 0);
    bytes_read = recv(fd, &(packet->buffer->size), sizeof(uint32_t), 0);
    packet->buffer->stream = malloc(packet->buffer->size);
    if (packet->buffer->size != 0)
        bytes_read = recv(fd, packet->buffer->stream, packet->buffer->size, 0);

    // no data was sent
    if (bytes_read == -1)
        return 0;
    // connection closed
    if (bytes_read == 0)
        return -1;

    // everything alright call the requestHandler
    requestHandler(fd, packet->op_code, packet->buffer, args);
    packet_free(packet);
    return 0;
};

int socket_createTcpServer(char *host, char *port)
{
    int fd;
    struct addrinfo *addr_info = getAddrInfo(NULL, port, SERVER);
    if (addr_info == NULL)
        return -1;

    struct addrinfo *addr;

    for (addr = addr_info; addr != NULL; addr = addr->ai_next)
    {
        fd = socket(addr->ai_family, addr->ai_socktype,
                    addr->ai_protocol);
        if (fd == -1)
            continue;

        if (bind(fd, addr->ai_addr, addr->ai_addrlen) == 0)
            break; /* Success */

        // this means that the we could not bind the socket, so close it and try next one.
        close(fd);
    }

    freeaddrinfo(addr_info);

    if (addr == NULL)
        return -1;

    int listening = listen(fd, SOMAXCONN);

    if (listening == -1)
        return -1;

    return fd;
}

int socket_acceptConns(int server_fd)
{
    struct sockaddr_in client_addr;
    socklen_t client_addrlen = sizeof(struct sockaddr_in);

    int client_fd = accept(server_fd, (void *)&client_addr, &client_addrlen);

    return client_fd;
}

/**
 * accept connections using an async non blocking io mechanism with epoll
 *
 * @returns `-1` if there is an error
 */
int socket_acceptConnsAsync(socket_AsyncServerConf *conf)
{
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
        return -1;

    for (int i = 0; i < conf->fds_size; i++)
    {
        if (setNonBlocking(*conf->server_fds[i]) == -1)
            return -1;
    }

    struct epoll_event event;
    memset(&event, 0, sizeof(event));

    // add the servers fds and listen to recv events
    for (int i = 0; i < conf->fds_size; i++)
    {
        event.data.fd = *conf->server_fds[i];
        event.events = EPOLLIN | EPOLLET;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, *conf->server_fds[i], &event) == -1)
            return -1;
    }

    // creates an array of size max_clients
    struct epoll_event *events = calloc(conf->max_clients, sizeof(event));

    for (;;)
    {
        int number_fds = epoll_wait(epoll_fd, events, conf->max_clients, -1);
        if (number_fds == -1)
        {
            free(events);
            return -1;
        }
        for (int i = 0; i < number_fds; i++)
        {
            // error
            if ((events[i].events & EPOLLERR) ||
                (!(events[i].events & EPOLLIN)))
            {
                continue;
            }
            // check if the event fd is from the server
            int is_server_fd = 0;
            for (int k = 0; k < conf->fds_size; k++)
            {
                int server_fd = *conf->server_fds[k];
                if (events[i].data.fd == server_fd)
                {
                    is_server_fd = 1;
                    // call accept as many times as we can
                    for (;;)
                    {
                        struct sockaddr client_addr;
                        socklen_t client_addrlen = sizeof(client_addr);
                        int client = accept(server_fd, &client_addr, &client_addrlen);
                        if (client == -1)
                        {
                            if (errno == EAGAIN || errno == EWOULDBLOCK)
                                // we processed all of the connections
                                break;
                            continue;
                        }
                        setNonBlocking(client);
                        event.events = EPOLLIN | EPOLLOUT;
                        event.data.fd = client;
                        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client, &event);
                        if (conf->onNewConnection != NULL)
                            conf->onNewConnection(client, conf->handlers_args);
                    };
                }
            }
            // handle client
            if (!is_server_fd)
            {
                int result = socket_read(events[i].data.fd, conf->requestHandler, conf->handlers_args);
                if (result == -1)
                {
                    if (conf->onConnectionClosed != NULL)
                        conf->onConnectionClosed(events[i].data.fd, conf->handlers_args);
                    // close connection
                    close(events[i].data.fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                }
            }
        }
    }
}

int socket_connectToServer(char *host, char *port)
{
    int fd;
    struct addrinfo *addr_info = getAddrInfo(NULL, port, CLIENT);

    if (addr_info == NULL)
        return -1;

    struct addrinfo *addr;

    for (addr = addr_info; addr != NULL; addr = addr->ai_next)
    {
        fd = socket(addr->ai_family, addr->ai_socktype,
                    addr->ai_protocol);
        if (fd == -1)
            continue;

        if (connect(fd, addr->ai_addr, addr->ai_addrlen) != -1)
            break;

        close(fd);
    }

    freeaddrinfo(addr_info);

    // no address succeeded
    if (addr == NULL)
        return -1;

    return fd;
}

void socket_freeConn(int socket_cliente)
{
    close(socket_cliente);
}

void server_listen(int fd, void (*connection_handler)(int *))
{
    int *client_fd;
    while (fd != -1)
    {
        client_fd = malloc(sizeof(int));
        *client_fd = socket_acceptConns(fd);
        if (*client_fd == -1)
        {
            free(client_fd);
            continue;
        }
        pthread_t client_thread;
        pthread_create(&client_thread, NULL, (void *)connection_handler, (void *)client_fd);
        pthread_detach(client_thread);
    }
}