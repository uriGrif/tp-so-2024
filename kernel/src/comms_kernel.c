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
        case IO_GEN_SLEEP:
        {
            uint32_t secs = packet_getUInt32(packet->buffer);
            log_info(logger, "voy a dormir %d segundos", secs);
            sleep(secs);
            t_packet* p = packet_new(DESTROY_PROCESS);
            packet_addString(p,"Hello from Kernel!");
            packet_send(p,client_fd);
            packet_free(p);
            break;
        }
        case -1:
            log_error(logger, "client disconnect");
            packet_free(packet);
            return;

        default:
            log_error(logger, "undefined behaviour with opcode: %d", packet->op_code);
            packet_free(packet);
            break;
        }
        packet_free(packet);
    }
}

void handle_connections(void * void_args){
    t_process_conn_args* args = (t_process_conn_args* )void_args;
    socket_acceptOnDemand(args->fd,args->logger,process_conn);
}