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

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = 0;
	
	socket_cliente = socket(server_info->ai_family,
							server_info->ai_socktype,
							server_info->ai_protocol);

	if(socket_cliente == -1){
		freeaddrinfo(server_info);
		return 0;
	}

	// Ahora que tenemos el socket, vamos a conectarlo

	if(connect(socket_cliente,server_info->ai_addr,server_info->ai_addrlen) == -1){
		freeaddrinfo(server_info);
		return 0;
	}
		

	freeaddrinfo(server_info);

	return socket_cliente;
}

void socket_freeConn(int socket_cliente)
{
    close(socket_cliente);
}