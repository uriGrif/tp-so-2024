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
    if(quantum_interruption_thread){
        pthread_cancel(quantum_interruption_thread);
        if(timer){
            temporal_stop(timer);
            time_elapsed = temporal_gettime(timer);
            temporal_destroy(timer);
        }  
    }
    handle_pause();
    // en todas le desalojo el contexto
    packet_get_context(packet->buffer, pcb->context);
    switch (packet->op_code)
    {
    case INTERRUPT_EXEC:
    {
        // cuando lo interrumpe por consola es aca
        move_pcb_to_exit(pcb, logger);
        break;
    }
    case END_OF_QUANTUM:
    {
        log_info(logger, "PID: %d - Desalojado por fin de Quantum", pcb->context->pid);
        scheduler.exec_to_ready(pcb, logger);
        print_ready_queue(logger);
        break;
    }
    case END_PROCESS:
    {
        log_info(logger, "Finaliza el proceso %d- Motivo: SUCCESS", pcb->context->pid);
        move_pcb_to_exit(pcb, logger);
        //  liberar de memoria
        break;
    }
    case WAIT:
    {
        char *resource_name = packet_getString(packet->buffer);
        t_blocked_queue *q = get_blocked_queue_by_name(resource_name);
        if (!q)
        {
            log_info(logger, "Finaliza el proceso %d- Motivo: No existe el recurso %s", pcb->context->pid, resource_name);
            move_pcb_to_exit(pcb, logger);
            free(resource_name);
            break;
        }
        q->instances--;
        if (q->instances < 0)
        {
            scheduler.move_pcb_to_blocked(pcb, q->resource_name, logger);
            log_info(logger, "PID: %d - Bloqueado por: %s", pcb->context->pid, resource_name);
            free(resource_name);
            break;
        }
        log_info(logger, "Instancias del recurso %s: %d", resource_name, q->instances);
        send_context_to_cpu(pcb->context); // creeria q en este caso se manda a ready
        scheduler.dispatch(pcb, logger);
        free(resource_name);
        break;
    }
    case SIGNAL:
    {
        char *resource_name = packet_getString(packet->buffer);
        t_blocked_queue *q = get_blocked_queue_by_name(resource_name);
        if (!q)
        {
            log_info(logger, "Finaliza el proceso %d- Motivo: No existe el recurso %s", pcb->context->pid, resource_name);
            move_pcb_to_exit(pcb, logger);
            free(resource_name);
            break;
        }
        q->instances++;
        if (q->instances <= 0)
        {
            pthread_mutex_lock(&MUTEX_LISTA_BLOCKEADOS);
            scheduler.block_to_ready(resource_name, logger);
            pthread_mutex_unlock(&MUTEX_LISTA_BLOCKEADOS);
            print_ready_queue(logger);
            sem_post(&scheduler.sem_ready);
        }
        send_context_to_cpu(pcb->context);
        scheduler.dispatch(pcb, logger);
        free(resource_name);
        break;
    }
    case IO_GEN_SLEEP:
    {
        t_interface *interface = interface_middleware(packet->buffer, IO_GEN_SLEEP, pcb, logger);
        if (!interface)
            break;
        t_interface_io_gen_sleep_msg *msg = malloc(sizeof(t_interface_io_gen_sleep_msg));
        interface_decode_io_gen_sleep(packet->buffer, msg);
        interface_send_io_gen_sleep(interface->fd, pcb->context->pid, msg->work_units);
        interface_destroy_io_gen_sleep(msg);
        break;
    }
    case IO_STDIN_READ:
    {
        t_interface *interface = interface_middleware(packet->buffer, IO_STDIN_READ, pcb, logger);
        if (!interface)
            break;
        t_interface_io_stdin_read_msg *msg = malloc(sizeof(t_interface_io_stdin_read_msg));
        interface_decode_io_stdin_read(packet->buffer, msg);
        interface_send_io_stdin_read(interface->fd, pcb->context->pid, msg->page_number, msg->offset, msg->size);
        interface_destroy_io_stdin_read(msg);
        break;
    }
    case IO_STDOUT_WRITE:
    {
        t_interface *interface = interface_middleware(packet->buffer, IO_STDOUT_WRITE, pcb, logger);
        if (!interface)
            break;
        t_interface_io_stdout_write_msg *msg = malloc(sizeof(t_interface_io_stdout_write_msg));
        interface_decode_io_stdout_write(packet->buffer, msg);
        interface_send_io_stdout_write(interface->fd, pcb->context->pid, msg->page_number, msg->offset, msg->size);
        interface_destroy_io_stdout_write(msg);
        break;
    }
    case READ_MEM_FAIL:{
        // desde la cpu, asumo que es buenito y se desaloja solo
         log_info(logger, "Finaliza el proceso %d- Motivo: INVALID READ", pcb->context->pid);
         move_pcb_to_exit(pcb,logger);
         break;
    }
     case WRITE_MEM_FAILED:{
        // desde la cpu, asumo que es buenito y se desaloja solo
         log_info(logger, "Finaliza el proceso %d- Motivo: INVALID WRITE", pcb->context->pid);
         move_pcb_to_exit(pcb,logger);
         break;
    }
    default:
        log_error(logger, "operacion desconocida opcode: %d", packet->op_code);
        break;
    }

    packet_free(packet);
    return 0;
}
