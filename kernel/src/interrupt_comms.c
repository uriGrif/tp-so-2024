#include<interrupt_comms.h>

int fd_interrupt;

void send_interrupt(t_pcb* pcb){
    t_packet* packet = packet_new(INTERRUPT_EXEC);
    packet_send(packet,fd_interrupt);
    packet_free(packet);
}