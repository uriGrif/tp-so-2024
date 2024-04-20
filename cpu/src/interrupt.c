#include <interrupt.h>

int interrupt_flag = 0;

void process_interrupt_conn(int fd, t_log *logger)
{
    while (1)
    {
        t_packet *packet = packet_new(0);
        if (packet_recv(fd, packet) == -1)
        {
            packet_free(packet);
            log_error(logger, "Se desconecto el kernel de interrupt\n"); // cambiar por log
            break;
        }
        interrupt_flag = 1;
        // log_debug(logger,"hola estoy aca");
        packet_free(packet);
    }
}

void handle_interrupt(void *_args)
{
    t_process_conn_args *args = _args;
    t_log *logger = args->logger;
    int server_fd = args->fd;

    free(args);

    while (1)
    {
        int client_fd = socket_acceptConns(server_fd);
        if (client_fd == -1)
            continue;

        process_interrupt_conn(client_fd, logger);
    }
}