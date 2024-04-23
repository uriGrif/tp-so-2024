#include <scheduler.h>

bool scheduler_paused = false;
sem_t sem_scheduler_paused;
t_scheduler scheduler;

 

void set_scheduling_algorithm(void){
    if(!strcmp(cfg_kernel->algoritmo_planificacion,"FIFO")){
        scheduler.ready_to_exec = ready_to_exec_fifo;
        scheduler.dispatch =  dispatch_fifo;
        return;
    }
    if(!strcmp(cfg_kernel->algoritmo_planificacion,"RR")){
        scheduler.ready_to_exec = ready_to_exec_rr;
        scheduler.dispatch =  dispatch_rr;
        return;
    }

        // scheduler.ready_to_exec = ready_to_exec_vrr;
        // scheduler.dispatch =  dispatch_vrr;
}




void handle_short_term_scheduler(void* args_logger) {
    t_log* logger = (t_log*) args_logger;
    set_scheduling_algorithm();
    
    while(1){
        if (!scheduler_paused) sem_wait(&sem_scheduler_paused);
        if(sync_queue_length(exec_queue) == 0) {
            t_pcb* pcb = scheduler.ready_to_exec();
            if(pcb){
                log_info(logger, "PID: %d - Estado Anterior: READY - Estado Actual: EXEC", pcb->context->pid);
                scheduler.dispatch(pcb,logger);
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