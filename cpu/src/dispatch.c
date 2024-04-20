#include<dispatch.h>

void send_dispatch_reason(t_opcode reason, t_exec_context* contxt){
    t_packet* packet = packet_new(reason);
    packet_add_context(packet,contxt);
    packet_send(packet,cli_dispatch_fd);
    packet_free(packet);
}