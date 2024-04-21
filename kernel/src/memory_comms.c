#include<memory_comms.h>

int fd_memory;

void send_create_process(t_pcb *pcb){
    t_packet* packet = packet_new(CREATE_PROCESS);
    packet_addUInt32(packet,pcb->context->pid);
    packet_addString(packet,pcb->text_path);
    packet_send(packet,fd_memory);
    packet_free(packet);
}

void send_end_process(uint32_t pid){
    t_packet* packet = packet_new(END_PROCESS);
    packet_addUInt32(packet,pid);
    packet_send(packet,fd_memory);
    packet_free(packet);
}