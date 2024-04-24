#include<round_robin.h>

t_pcb* ready_to_exec_rr(void){
    return ready_to_exec_fifo();
}

void dispatch_rr(t_pcb* pcb, t_log* logger){
    // manejar el quantum send interrupt etc etc
}

void block_to_ready_rr(char* resource,t_log* logger){
    // buscar la cola por nombre de recurso
    // popear el primero (EN DUDA)
    // pasar a ready
    block_to_ready_fifo(resource,logger);
}