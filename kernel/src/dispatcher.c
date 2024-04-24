#include <dispatcher.h>

int fd_dispatch;

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
    case END_PROCESS:
    {
        log_info(logger, "me llego PID: %d AX: %d", pcb->context->pid, pcb->context->registers.ax);
        pcb_destroy(pcb);
        // tocar grado multiprogramacion
        //  actualizar context del pcb
        //  mandar proceso a exit
        //  liberar de memoria
        break;
    }
    case IO_GEN_SLEEP:
    {
        struct req_io_gen_sleep *params = malloc(sizeof(struct req_io_gen_sleep));
        interface_decode_io_gen_sleep(packet->buffer, params);
        t_interface *interface = interface_validate(params->interface_name, IO_GEN_SLEEP);
        if (!interface)
        {
            log_error(logger, "Validation for interface with name %s for instruction %s failed", params->interface_name, "IO_GEN_SLEEP");
            // mandar proceso a exit
            interface_destroy_io_gen_sleep(params);
            break;
        }
        if (move_pcb_to_blocked(pcb, interface->name) == -1)
        {
            log_error(logger, "Could not find blocked queue for %s", params->interface_name);
            // mandar proceso a exit
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

int wait_for_context_no_reason(t_pcb *pcb)
{
    t_packet *packet = packet_new(-1);
    if (packet_recv(fd_dispatch, packet) == -1)
    {
        packet_free(packet);
        return -1;
    }

    packet_get_context(packet->buffer, pcb->context);
    packet_free(packet);
    return 0;
}