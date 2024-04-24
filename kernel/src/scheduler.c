#include <scheduler.h>

bool scheduler_paused = false;
sem_t sem_scheduler_paused;
sem_t sem_ready;
t_scheduler scheduler;


void init_scheduler_sems(void){
    sem_init(&sem_scheduler_paused,0,0);
    sem_init(&sem_ready,0,0);
}

void destroy_scheduler_sems(void){
    sem_destroy(&sem_ready);
    sem_destroy(&sem_scheduler_paused);
}

static void set_scheduling_algorithm(void)
{
    if (!strcmp(cfg_kernel->algoritmo_planificacion, "FIFO"))
    {
        scheduler.ready_to_exec = ready_to_exec_fifo;
        scheduler.dispatch = dispatch_fifo;
        scheduler.block_to_ready = block_to_ready_fifo;
        return;
    }
    if (!strcmp(cfg_kernel->algoritmo_planificacion, "RR"))
    {
        scheduler.ready_to_exec = ready_to_exec_rr;
        scheduler.dispatch = dispatch_rr;
        scheduler.block_to_ready = block_to_ready_rr;
        return;
    }

    // scheduler.ready_to_exec = ready_to_exec_vrr;
    // scheduler.dispatch =  dispatch_vrr;
}

void handle_short_term_scheduler(void *args_logger)
{
    t_log *logger = (t_log *)args_logger;
    set_scheduling_algorithm();

    while (1)
    {
        if (scheduler_paused)
            sem_wait(&sem_scheduler_paused);
        if (sync_queue_length(exec_queue) == 0)
        {
            sem_wait(&sem_ready);
            t_pcb *pcb = scheduler.ready_to_exec();
            if (pcb)
            {
                log_info(logger, "PID: %d - Estado Anterior: READY - Estado Actual: EXEC", pcb->context->pid);
                scheduler.dispatch(pcb, logger);
                t_pcb* tmp = queue_sync_pop(exec_queue); // solo lo saco, la referencia creo que ya la tengo
                pcb_destroy(tmp); // para no perder memoria por ahora pero no va
            }
        }
    }
}

void handle_long_term_scheduler(void *args_logger)
{
    return;
    //while (1)
    //{
        // while ( haya procesos en new)
        // {
        //      lo paso a ready
        //      pcb->state = READY;
        // }

        // while ( haya procesos en exit)
        // {
        //      libero los recursos
        // }
    //}
}