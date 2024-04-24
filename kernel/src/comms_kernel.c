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
            log_warning(logger, "client disconnect");
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
            log_info(logger, "New interface registered: name: %s - type: %s", interface->name, interface->type);
            break;
        }
        case IO_DONE:
        {
            char* resource_name = packet_getString(packet->buffer);
            log_info(logger,"Interface %s done",resource_name);
            scheduler.block_to_ready(resource_name,logger);
            free(resource_name);
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