#include <scheduler.h>

static void set_scheduling_algorithm(void);


bool scheduler_paused = true;
t_scheduler scheduler;

static void init_scheduler_sems(void)
{
    sem_init(&scheduler.sem_scheduler_paused, 0, 0);
    sem_init(&scheduler.sem_ready, 0, 0);
    sem_init(&scheduler.sem_new, 0, 0);
}

static void destroy_scheduler_sems(void)
{
    sem_destroy(&scheduler.sem_ready);
    sem_destroy(&scheduler.sem_scheduler_paused);
    sem_destroy(&scheduler.sem_new);
}

void init_scheduler(void)
{
    init_scheduler_sems();
    set_scheduling_algorithm();
}

void destroy_scheduler(void)
{
    destroy_scheduler_sems();
}

static void set_scheduling_algorithm(void)
{
    if (!strcmp(cfg_kernel->algoritmo_planificacion, "FIFO"))
    {
        scheduler.ready_to_exec = ready_to_exec_fifo;
        scheduler.dispatch = dispatch_fifo;
        scheduler.block_to_ready = block_to_ready_fifo;
        scheduler.move_pcb_to_blocked = move_pcb_to_blocked_fifo;
        return;
    }
    if (!strcmp(cfg_kernel->algoritmo_planificacion, "RR"))
    {
        scheduler.ready_to_exec = ready_to_exec_rr;
        scheduler.dispatch = dispatch_rr;
        scheduler.block_to_ready = block_to_ready_rr;
        scheduler.exec_to_ready = exec_to_ready_rr;
        scheduler.move_pcb_to_blocked = move_pcb_to_blocked_rr;
        return;
    }
    // scheduler.ready_to_exec = ready_to_exec_vrr;
    // scheduler.dispatch =  dispatch_vrr;
    //scheduler.exec_to_ready = exec_to_ready_vrr;
}

void handle_short_term_scheduler(void *args_logger)
{
    t_log *logger = (t_log *)args_logger;
    set_scheduling_algorithm();

    while (1)
    {
        if (scheduler_paused)
            sem_wait(&scheduler.sem_scheduler_paused);
        sem_wait(&scheduler.sem_ready);
        t_pcb *pcb = scheduler.ready_to_exec();
        log_info(logger, "PID: %d - Estado Anterior: READY - Estado Actual: EXEC", pcb->context->pid); // solo lo saco, la referencia creo que ya la tengo
        scheduler.dispatch(pcb, logger);
        queue_sync_pop(exec_queue);
        // para no perder memoria por ahora pero no va
    }
}

// int move_pcb_to_blocked(t_pcb *pcb, char *resource_name, t_log *logger)
// {
//     if(blocked_queue_push(resource_name, pcb) == -1)
//         return -1;
//     pcb->state = BLOCKED;
//     log_info(logger, "PID: %d - Estado Anterior: EXEC - Estado Actual: BLOCKED", pcb->context->pid);
//     return 0;
// }

void move_pcb_to_exit(t_pcb *pcb, t_log *logger)
{
    queue_sync_push(exit_queue, pcb);
    log_info(logger, "PID: %d - Estado Anterior: %s - Estado Actual: EXIT", pcb->context->pid, pcb_state_to_string(pcb));
    pcb->state = EXIT;
    send_end_process(pcb->context->pid);
}

void handle_long_term_scheduler(void *args_logger)
{
    return;
    // while (1)
    //{
    //  while ( haya procesos en new)
    //  {
    //       lo paso a ready
    //       pcb->state = READY;
    //  }

    // while ( haya procesos en exit)
    // {
    //      libero los recursos
    // }
    //}
}