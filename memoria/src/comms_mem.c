#include <comms_mem.h>

void process_conn(void *void_args)
{
    t_process_conn_args *args = (t_process_conn_args *)void_args;
    t_log *logger = args->logger;
    int client_fd = args->fd;
    free(args);

    t_packet *packet = packet_new(0);

    while (client_fd != -1)
    {
        if (packet_recv(client_fd, packet) == -1)
        {
            packet_free(packet);
            return;
        }
        switch (packet->op_code)
        {
        case SAVE_CONTEXT:
        {
            uint32_t x = packet_getUInt32(packet->buffer);
            uint32_t y = packet_getUInt32(packet->buffer);
            log_info(logger, "nro 1 : %d, nro 2: %d", x, y);
            break;
        }
        case READ_MEM:
        {
            char *result = packet_getString(packet->buffer);
            log_info(logger, "me llego: %s", result);
            free(result);
            break;
        }
        case CREATE_PROCESS:
        {
            log_info(logger, "CREATE PROCESS");
            char **result = packet_get_string_arr(packet->buffer);

            void print_str(char *str)
            {
                printf("%s ", str);
            }
            log_info(logger, "procedo a leer el array de strings que me llego");
            string_iterate_lines(result, print_str);
            printf("\n");
            string_array_destroy(result);
            break;
        }
        case -1:
            log_error(logger, "client disconnect");
            packet_free(packet);
            return;

        default:
            log_error(logger, "undefined behaviour with opcode: %d", packet->op_code);
            packet_free(packet);
            return;
        }
        log_warning(logger, "client disconnect");
        packet_free(packet);
        return;
    }
}