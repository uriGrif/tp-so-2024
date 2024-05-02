#include <comms_kernel.h>

void process_conn(void *void_args)
{
    t_process_conn_args *args = (t_process_conn_args *)void_args;
    t_log *logger = args->logger;
    int client_fd = args->fd;
    free(args);
    // tengo que hacer esto porque aca se crean n hilos on demanda
    // por ahora si conecto las interfaces antes de iniciar la planificacion por primera vez no funciona bien
    // pero me parece que en la entrega posta arranca con la planificacion despausada
    sem_t sem_scheduler_pause;
    sem_init(&sem_scheduler_pause, 0, 0);
    list_add(scheduler.sems_scheduler_paused, &sem_scheduler_pause);

    while (client_fd != -1)
    {
        t_packet *packet = packet_new(0);
        if (packet_recv(client_fd, packet) == -1) // SE DESCONECTA ENTONCES
        {
            // t_sync_queue* blocked_queue = get_blocked_queue_by_fd(client_fd);
            //  deberia mandar todos a exit pero por ahora...
            t_interface *interface = interface_get_by_fd(client_fd);
            if (interface)
            {
                log_warning(logger, "Interfaz %s de tipo %s se desconecto, todos los bloqueados por ella iran a exit", interface->name, interface->type);
                interface_destroy(interface);
            }
            else
                log_debug(logger, "no se conecto de una");

            if (scheduler_paused)
                sem_wait(&sem_scheduler_pause);
            pthread_mutex_lock(&MUTEX_LISTA_BLOCKEADOS);
            t_blocked_queue *block_queue_to_remove = get_blocked_queue_by_fd(client_fd);
            void iterator(void *elem)
            {
                t_pcb *pcb = (t_pcb *)elem;
                log_info(logger, "Finaliza el proceso %d - Motivo: Error desconexion de interfaz", pcb->context->pid);
                move_pcb_to_exit(pcb, logger);
            }
            // mando todos los procesos de esa cola a exit y elimino la cola mas la interfaz
            if (block_queue_to_remove)
            {
                sync_queue_iterate(block_queue_to_remove->block_queue, iterator);
                remove_blocked_queue_by_fd(client_fd);
            }
            packet_free(packet);
            pthread_mutex_unlock(&MUTEX_LISTA_BLOCKEADOS);
            return;
        }
        log_debug(logger, "pase por aca");
        switch (packet->op_code)
        {
        case NEW_INTERFACE:
        {
            t_interface *interface = malloc(sizeof(t_interface));
            interface->fd = client_fd;
            interface->name = packet_getString(packet->buffer);
            interface->type = packet_getString(packet->buffer);
            interface_add(interface);
            add_blocked_queue(interface->name, client_fd);
            log_info(logger, "New interface registered: name: %s - type: %s", interface->name, interface->type);
            break;
        }
        case IO_DONE:
        {
            t_interface_io_done_msg *msg = malloc(sizeof(t_interface_io_done_msg));
            interface_decode_io_done(packet->buffer, msg);
            log_info(logger, "Interface %s requested by pid %d done", msg->interface_name, msg->pid);
            if (scheduler_paused)
                sem_wait(&sem_scheduler_pause);
            pthread_mutex_lock(&MUTEX_LISTA_BLOCKEADOS);
            scheduler.block_to_ready(msg->interface_name, logger);
            pthread_mutex_unlock(&MUTEX_LISTA_BLOCKEADOS);
            print_ready_queue(logger);
            sem_post(&scheduler.sem_ready);
            interface_destroy_io_done(msg);
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

void handle_connections(void *void_args)
{
    t_process_conn_args *args = (t_process_conn_args *)void_args;
    socket_acceptOnDemand(args->fd, args->logger, process_conn);
}