#include <dispatch.h>

void process_dispatch_conn(void *void_args)
{
    t_process_conn_args *args = (t_process_conn_args *)void_args;
    t_log *logger = args->logger;
    int client_fd = args->fd;
    free(args);

    while (client_fd != -1)
    {
        t_packet *packet = packet_new(0);
        if (packet_recv(client_fd, packet) == -1)
        {
            log_error(logger, "Se desconecto el kernel de dispatch\n");
            packet_free(packet);
            return;
        }
        switch (packet->op_code)
        {
        case EXEC_PROCESS:
        {
            log_info(logger, "EXEC PROCESS\n");
            char *str = packet_getString(packet->buffer);
            uint32_t value = packet_getUInt32(packet->buffer);
            char *str2 = packet_getString(packet->buffer);
            log_info(logger, "string: %s\n", str);
            log_info(logger, "number: %d\n", value);
            log_info(logger, "string2: %s\n", str2);
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

        packet_free(packet);
    }
}