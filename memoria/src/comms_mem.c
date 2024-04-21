#include <comms_mem.h>
#include <utils.h>

void process_conn(void *void_args)
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
            log_warning(logger, "client disconnect");
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
            uint32_t pid = packet_getUInt32(packet->buffer);
            char *text_path = packet_getString(packet->buffer);
            log_info(logger, "CREATE PROCESS with pid: %d path: %s", pid, text_path);
            free(text_path);
            break;
        }
        case END_PROCESS:
        {
            uint32_t pid = packet_getUInt32(packet->buffer);
            log_info(logger, "ENDING PROCESS with pid: %d", pid);
            break;
        }
        case NEXT_INSTRUCTION:
        {
            log_debug(logger, "NEXT_INSTRUCTION");
            uint32_t pid = packet_getUInt32(packet->buffer);
            uint32_t pc = packet_getUInt32(packet->buffer);
            // BUSCO CUAL DEL LOS ARCHIVOS ESTA EN EXEC
            char *text_name = mount_instructions_directory("ejemplo1.txt");
            char *next_instruction = file_get_nth_line(text_name, pc);
            free(text_name);
            if (!next_instruction)
            {
                // mandame un error o algo no se
                log_warning(logger, "no hay proxima instruccion");
                break;
            }
            packet_addString(packet, next_instruction);
            packet_send(packet, client_fd);
            free(next_instruction);
            break;
        }
        case -1:
            log_error(logger, "client disconnect");
            packet_free(packet);
            return;

        default:
            log_error(logger, "undefined behaviour with opcode: %d", packet->op_code);
            break;
        }
        packet_free(packet);
    }
}