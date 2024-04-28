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
            t_process_in_mem *process = t_process_in_mem_create();
            if(!process){
                log_error(logger,"not enough space for creating process");
                break;
            }
            packet_get_process_in_mem(packet->buffer, process);
            char* path = mount_instructions_directory(process->path);
            if(!file_exists(path)){
                log_error(logger,"el archivo de instrucciones de este archivo no fue encontrado");
                free(path);
                t_process_in_mem_destroy(process);
                break;
            }
            free(path);
            log_info(logger, "CREATE PROCESS with pid: %d - path: %s", process->pid, process->path);
            add_process(process);
            break;
        }
        case END_PROCESS:
        {
            uint32_t pid = packet_getUInt32(packet->buffer);
            remove_process_by_pid(pid);
            log_info(logger, "ENDING PROCESS with pid: %d", pid);
            break;
        }
        case NEXT_INSTRUCTION:
        {
            msleep(cfg_mem->retardo_respuesta);
            log_debug(logger, "NEXT_INSTRUCTION");
            uint32_t pid = packet_getUInt32(packet->buffer);
            uint32_t pc = packet_getUInt32(packet->buffer);
            // BUSCO CUAL DEL LOS ARCHIVOS ESTA EN EXEC
            t_process_in_mem* process = find_process_by_pid(pid);
            if(!process){
                log_error(logger,"process with pid %d not found",pid);
                break;
            }
            char *text_name = mount_instructions_directory(
                process->path
            );
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