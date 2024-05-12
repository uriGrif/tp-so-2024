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
        case CPU_HANDSHAKE:{
            packet_addUInt32(packet,cfg_mem->tam_pagina);
            packet_send(packet,client_fd);
            break;
        }
        case SAVE_CONTEXT:
        {
            uint32_t x = packet_getUInt32(packet->buffer);
            uint32_t y = packet_getUInt32(packet->buffer);
            log_info(logger, "nro 1 : %d, nro 2: %d", x, y);
            break;
        }
        case READ_MEM:
        {
            t_memory_read_msg *msg = malloc(sizeof(t_memory_read_msg));
            memory_decode_read(packet->buffer, msg);
            log_info(logger, "PID : %d - Accion : LEER - Numero de pagina : %d - Desplazamiento %d", msg->pid, msg->page_number, msg->offset);

            // arbitrary test value
            char *str = string_substring_until("hello boy, how are you doing?",msg->size);
            memory_send_read_ok(client_fd, str, msg->size);
            free(str);

            memory_destroy_read(msg);
            break;
        }
        case WRITE_MEM:
        {
            t_memory_write_msg *msg = malloc(sizeof(t_memory_write_msg));
            memory_decode_write(packet->buffer, msg);

            log_info(logger, "PID : %d - Accion : ESCRIBIR - Numero de pagina : %d - Desplazamiento %d", msg->pid, msg->page_number, msg->offset);

            log_info(logger, "DATA TO SAVE: %s", (char *)msg->value);

            memory_send_write_ok(client_fd);

            memory_destroy_write(msg);
            break;
        }
        case CREATE_PROCESS:
        {
            t_process_in_mem *process = t_process_in_mem_create();
            if (!process)
            {
                log_error(logger, "not enough space for creating process");
                break;
            }
            packet_get_process_in_mem(packet->buffer, process);
            if (!file_exists(process->path))
            {
                log_error(logger, "el archivo de instrucciones de este archivo no fue encontrado");
                t_process_in_mem_destroy(process);
                break;
            }
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
            t_process_in_mem *process = find_process_by_pid(pid);
            if (!process)
            {
                log_error(logger, "process with pid %d not found", pid);
                break;
            }
            char *next_instruction = file_get_nth_line(process->path, pc);
            if (!next_instruction)
            {
                // mandame un error o algo no se
                log_warning(logger, "no hay proxima instruccion");
                packet->op_code = NO_INSTRUCTION;
                packet_send(packet,client_fd);
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