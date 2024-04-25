#include <comms_kernel.h>

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
            // t_sync_queue* blocked_queue = get_blocked_queue_by_fd(client_fd);
            //  deberia mandar todos a exit pero por ahora...
            t_interface *interface = interface_get_by_fd(client_fd);
            if (interface)
            {
                log_warning(logger, "Interfaz %s de tipo %s se desconecto", interface->name, interface->type);
                interface_destroy(interface);
            }
            t_sync_queue *block_queue_to_remove = get_blocked_queue_by_fd(client_fd);
            void iterator(void *elem)
            {
                t_pcb *pcb = (t_pcb *)elem;
                move_pcb_to_exit(pcb, logger);
            }
            // mando todos los procesos de esa cola a exit y elimino la cola mas la interfaz
            if (block_queue_to_remove)
            {
                sync_queue_iterate(block_queue_to_remove, iterator);
                remove_blocked_queue_by_fd(client_fd);
            }

            packet_free(packet);
            return;
        }
        switch (packet->op_code)
        {
        case NEW_INTERFACE:
        {
            t_interface *interface = malloc(sizeof(t_interface));
            interface->fd = client_fd;
            interface_decode_new(packet->buffer, interface);
            interface_add(interface);
            add_blocked_queue(interface->name, client_fd);
            log_info(logger, "New interface registered: name: %s - type: %s", interface->name, interface->type);
            break;
        }
        case IO_DONE:
        {
            uint32_t pid = packet_getUInt32(packet->buffer);
            char *resource_name = packet_getString(packet->buffer);
            log_info(logger, "Interface %s done", resource_name);
            t_sync_queue *q = get_blocked_queue_by_name(resource_name);
            if (q)
            {
                t_pcb *pcb = sync_queue_peek(q, 0);
                if (pcb->context->pid == pid)
                    scheduler.block_to_ready(resource_name, logger);
                free(resource_name);
            }
            // now we should move the process waiting for this i/o to finish from blocked to ready.
            // uint32_t pid = packet_getUInt32(packet->buffer);
            // log_info(logger, "I/O operation for process with PID %d has finished", pid);
            // pasar de block a ready
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