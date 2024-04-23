#include <scheduler.h>

bool scheduler_paused = false;
sem_t sem_scheduler_paused;
typedef t_pcb* (*ready_to_exec_strategy)(void);

ready_to_exec_strategy r_to_ex_strategy; // para vos escobar


t_pcb* ready_to_exec(void){
    t_pcb* pcb = queue_sync_pop(ready_queue);
    if(pcb){
        queue_sync_push(exec_queue,pcb);
        pcb->state = EXEC;
        send_context_to_cpu(pcb->context);
    }
    return pcb;
}

void handle_short_term_scheduler(void* args_logger) {
    t_log* logger = (t_log*) args_logger;
    r_to_ex_strategy = ready_to_exec;
    
    while(1){
        if (!scheduler_paused) sem_wait(&sem_scheduler_paused);
        if(sync_queue_length(exec_queue) == 0) {
            t_pcb* pcb = r_to_ex_strategy();
            if(pcb){
                log_info(logger, "PID: %d - Estado Anterior: READY - Estado Actual: EXEC", pcb->context->pid);
                wait_for_dispatch_reason(pcb,logger);
            } 
            //popeo me guardo el pcb aca
        }
    }
}

void handle_long_term_scheduler(void* args_logger) {
    while (1)
    {
        // while ( haya procesos en new)
        // {
        //      lo paso a ready
        //      pcb->state = READY;
        // }

        // while ( haya procesos en exit)
        // {
        //      libero los recursos
        // }
        
    }
}