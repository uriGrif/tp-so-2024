#include <dispatcher.h>

int fd_dispatch;
// whether the interruptiion was due to a i/o instruction or not
bool DONE_BEFORE_QUANTUM = false;

void send_context_to_cpu(t_exec_context *context)
{
    t_packet *packet = packet_new(EXEC_PROCESS); // puede ser otro no importa
    packet_add_context(packet, context);
    packet_send(packet, fd_dispatch);
    packet_free(packet);
}

int wait_for_dispatch_reason(t_pcb *pcb, t_log *logger)
{
    t_packet *packet = packet_new(-1);
    if (packet_recv(fd_dispatch, packet) == -1)
    {
        packet_free(packet);
        return -1;
    }
    // en todas le desalojo el contexto
    packet_get_context(packet->buffer, pcb->context);
    switch (packet->op_code)
    {
    case INTERRUPT_EXEC: // FIN DE QUANTUM
    {
        if (pcb->state == EXIT)
            break;
        log_debug(logger,"me lleg por interrupt aca");
        scheduler.exec_to_ready(pcb, logger);
        break;
    }
    case END_PROCESS:
    {
        DONE_BEFORE_QUANTUM = true;
        log_debug(logger, "me llego PID: %d AX: %d", pcb->context->pid, pcb->context->registers.ax);
        move_pcb_to_exit(pcb, logger);
        //  liberar de memoria
        break;
    }
    case IO_GEN_SLEEP:
    {
        DONE_BEFORE_QUANTUM = true;
        struct req_io_gen_sleep *params = malloc(sizeof(struct req_io_gen_sleep));
        interface_decode_io_gen_sleep(packet->buffer, params);
        t_interface *interface = interface_validate(params->interface_name, IO_GEN_SLEEP);
        if (!interface)
        {
            log_error(logger, "Validation for interface with name %s for instruction %s failed", params->interface_name, "IO_GEN_SLEEP");
            // mandar proceso a exit
            interface_destroy_io_gen_sleep(params);
            // nunca pase por bloqueado asi que no deberia explotar
            move_pcb_to_exit(pcb, logger);
            break;
        }
        if (scheduler.move_pcb_to_blocked(pcb, interface->name, logger) == -1)
        {
            log_error(logger, "Could not find blocked queue for %s", params->interface_name);
            move_pcb_to_exit(pcb, logger);
            break;
        }
        // here we would get the current exec pcb and move it to the blocked queue. that means we need to send a deallocation
        log_info(logger, "PID: %d - Bloqueado por: %s", pcb->context->pid, params->interface_name);
        interface_send_io_gen_sleep(interface->fd, pcb->context->pid, params->work_units);
        interface_destroy_io_gen_sleep(params);
        break;
    }
    default:
        log_error(logger, "operacion desconocida opcode: %d", packet->op_code);
        break;
    }

    packet_free(packet);
    return 0;
}
