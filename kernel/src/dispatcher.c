#include <dispatcher.h>

int fd_dispatch;
// whether the interruptiion was due to a i/o instruction or not


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
    sem_t* sem_scheduler_pause = list_get(scheduler.sems_scheduler_paused,0);
    // en todas le desalojo el contexto
    if(quantum_interruption_thread) 
        pthread_cancel(quantum_interruption_thread);
    // aca puedo frenar el timer para el vrr
    if (scheduler_paused)
            sem_wait(sem_scheduler_pause);
    packet_get_context(packet->buffer, pcb->context);
    switch (packet->op_code)
    {
    case INTERRUPT_EXEC:
    {
        // cuando lo interrumpe por consola es aca
        move_pcb_to_exit(pcb,logger);
        break;
    }
    case END_OF_QUANTUM:{
        log_info(logger,"PID: %d - Desalojado por fin de Quantum",pcb->context->pid);
        scheduler.exec_to_ready(pcb, logger);
        print_ready_queue(logger);
        break;
    }
    case END_PROCESS:
    {
        log_info(logger, "Finaliza el proceso %d- Motivo: SUCCESS",pcb->context->pid);
        move_pcb_to_exit(pcb, logger);
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
            //log_error(logger, "Validation for interface with name %s for instruction %s failed", params->interface_name, "IO_GEN_SLEEP");
            log_info(logger, "Finaliza el proceso %d- Motivo: Error de interfaz %s no conectada",pcb->context->pid,params->interface_name);
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
