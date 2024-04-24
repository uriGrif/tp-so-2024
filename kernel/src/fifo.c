#include<fifo.h>

t_pcb* ready_to_exec_fifo(void){
    t_pcb* pcb = queue_sync_pop(ready_queue);
    if(pcb){
        queue_sync_push(exec_queue,pcb);
        pcb->state = EXEC;
        send_context_to_cpu(pcb->context);
    }
    return pcb;
}


void dispatch_fifo(t_pcb* pcb, t_log* logger){
    if(wait_for_dispatch_reason(pcb,logger) == -1){
        log_error(logger,"error waiting for cpu context");
    }
}

void block_to_ready_fifo(char* resource,t_log* logger){

    // buscar la cola por nombre de recurso
    // popear el primero (EN DUDA)
    // pasar a ready
    log_info(logger, "PID: %d - Estado Anterior: BLOCKED - Estado Actual: READY",1); //pcb->context->pid
}

