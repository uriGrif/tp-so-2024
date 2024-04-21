#include <dispatcher.h>

int fd_dispatch;

void send_context_to_cpu(t_exec_context *context)
{
    t_packet *packet = packet_new(EXEC_PROCESS); // puede ser otro no importa
    packet_add_context(packet,context);
    packet_send(packet, fd_dispatch);
    packet_free(packet);
}

int wait_for_dispatch_reason(t_log *logger)
{
    t_packet *packet = packet_new(-1);
    if (packet_recv(fd_dispatch, packet) == -1)
    {
        packet_free(packet);
        return -1;
    }

    switch (packet->op_code)
    {
    case END_PROCESS:
    {
        t_exec_context *contxt = malloc(sizeof(t_exec_context));
        packet_get_context(packet->buffer, contxt);
        log_info(logger, "me llego PID: %d AX: %d", contxt->pid, contxt->registers.ax);
        free(contxt);
        // tocar grado multiprogramacion
        //  actualizar context del pcb
        //  mandar proceso a exit
        //  liberar de memoria
        break;
    }
    case IO_GEN_SLEEP:
    {
        t_exec_context *contxt = malloc(sizeof(t_exec_context));
        packet_get_context(packet, &contxt);
        free(contxt);
        // tocar grado multiprogramacion
        //  actualizar context del pcb
        //  pasar proceso a blocked
        // buscar interfaz si no esta mando a exit
        //  intentar hacer sleep y si no coincide lo mando a exit
        //  hacer el sleep
    }
    default:
        log_error(logger, "operacion desconocida opcode: %d", packet->op_code);
        break;
    }

    packet_free(packet);
}

int wait_for_context_no_reason(t_pcb* pcb){
    t_packet *packet = packet_new(-1);
    if (packet_recv(fd_dispatch, packet) == -1)
    {
        packet_free(packet);
        return -1;
    }

    packet_get_context(packet->buffer,pcb->context);
    packet_free(packet);
    return 0;
}