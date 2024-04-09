#include <sockets/sockets.h>

int socket_createTcpServer(char *host, char *port)
{
    int fd;

    struct addrinfo hints, *server_info;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;

    getaddrinfo(host, port, &hints, &server_info);

    struct addrinfo *addr;

    for (addr = server_info; addr != NULL; addr = addr->ai_next)
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

    freeaddrinfo(server_info);

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

int socket_connectToServer(char *host, char *port)
{
    struct addrinfo hints;
    struct addrinfo *server_info;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;

    getaddrinfo(host, port, &hints, &server_info);

    int fd = 0;
    struct addrinfo *addr;

    for (addr = server_info; addr != NULL; addr = addr->ai_next)
    {
        fd = socket(addr->ai_family, addr->ai_socktype,
                    addr->ai_protocol);
        if (fd == -1)
            continue;

        if (connect(fd, addr->ai_addr, addr->ai_addrlen) != -1)
            break;

        close(fd);
    }

    freeaddrinfo(server_info);

    // no address succeeded
    if (addr == NULL)
        return -1;

    return fd;
}

void socket_freeConn(int socket_cliente)
{
    close(socket_cliente);
}

void server_listen(int fd,t_log* logger, void (*connection_handler)(void*)){
    while(fd != -1){
        int client_fd = socket_acceptConns(fd);
        if(client_fd == -1)
            continue;

        t_process_conn_args* args = malloc(sizeof(t_process_conn_args));
        args->fd = client_fd;
        args->logger = logger;
        pthread_t client_thread;
        pthread_create(&client_thread,NULL,(void*)connection_handler,(void *)args);
        pthread_detach(client_thread);
    }
}