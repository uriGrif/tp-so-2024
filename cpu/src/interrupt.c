#include <interrupt.h>

int interrupt_flag = 0;
sem_t sem_interrupt;
sem_t sem_interrupt_done;

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
        sem_wait(&sem_interrupt);
        switch (packet->op_code)
        {
        case INTERRUPT_EXEC:
        {
            log_info(logger, "INTERRUPT EXEC\n");
            char *str = packet_getString(packet->buffer);
            uint32_t value = packet_getUInt32(packet->buffer);
            char *str2 = packet_getString(packet->buffer);
            log_info(logger, "string: %s\n", str);
            log_info(logger, "number: %d\n", value);
            log_info(logger, "string2: %s\n", str2);
            send_dispatch_reason(END_PROCESS, &context);
            free(str);
            free(str2);
            break;
        }
        case -1:
            log_error(logger, "client disconnect");
            packet_free(packet);
            return;

        default:
            log_error(logger, "undefined behaviour cop: %d", packet->op_code);
            break;
        }
        // log_debug(logger,"hola estoy aca");
        packet_free(packet);
        sem_post(&sem_interrupt_done);
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

void init_sem_interrupt()
{
    sem_init(&sem_interrupt, 0, 0);
    sem_init(&sem_interrupt_done, 0, 0);
}

void close_sem_interrupt()
{
    sem_destroy(&sem_interrupt);
    sem_destroy(&sem_interrupt_done);
}