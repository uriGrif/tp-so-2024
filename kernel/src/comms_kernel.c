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
        }
        case IO_GEN_SLEEP:
        {
            struct req_io_gen_sleep *params = malloc(sizeof(struct req_io_gen_sleep));
            interface_decode_io_gen_sleep(packet->buffer, params);
            t_interface *interface = interface_validate(params->interface_name, IO_GEN_SLEEP);
            if (interface == NULL)
            {
                interface_destroy(interface);
                log_error(logger, "Validation for interface with name %s for instruction %d failed", params->interface_name, IO_GEN_SLEEP);
                // todo send process to exit
                break;
            }
            // here we would get the current exec pcb and move it to the blocked queue. that means we need to send a deallocation
            log_error(logger, "PID: %d - Bloqueado por: < %s / %s>", params->interface_name, IO_GEN_SLEEP);
            int pid = 10;
            interface_send_io_gen_sleep(interface->fd, pid, params->work_units);
            free(params);
        }
        case IO_GEN_SLEEP_DONE:
        {
            // now we should move the process waiting for this i/o to finish from blocked to ready.
            uint32_t pid = packet_getUInt32(packet->buffer);
            log_info(logger, "I/O operation for process with PID %d has finished", pid);
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