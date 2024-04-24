#include <scheduler.h>

static void set_scheduling_algorithm(void);

bool scheduler_paused = true;
t_scheduler scheduler;

static void init_scheduler_sems(void)
{
    sem_init(&scheduler.sem_scheduler_paused, 0, 0);
    sem_init(&scheduler.sem_ready, 0, 0);
}

static void destroy_scheduler_sems(void)
{
    sem_destroy(&scheduler.sem_ready);
    sem_destroy(&scheduler.sem_scheduler_paused);
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
            sem_wait(&scheduler.sem_scheduler_paused);
        if (sync_queue_length(exec_queue) == 0)
        {
            sem_wait(&scheduler.sem_ready);
            t_pcb *pcb = scheduler.ready_to_exec();
            if (pcb)
            {
                log_info(logger, "PID: %d - Estado Anterior: READY - Estado Actual: EXEC", pcb->context->pid);
                scheduler.dispatch(pcb, logger);
                queue_sync_pop(exec_queue); // solo lo saco, la referencia creo que ya la tengo
                // para no perder memoria por ahora pero no va
            }
        }
    }
}

int move_pcb_to_blocked(t_pcb *pcb, char *resource_name,t_log* logger)
{
    t_sync_queue *tmp;
    if ((tmp = get_blocked_queue_by_name(resource_name)))
    {
        queue_sync_push(tmp, pcb);
        pcb->state = BLOCKED;
          log_info(logger, "PID: %d - Estado Anterior: EXEC - Estado Actual: BLOCKED", pcb->context->pid);
        return 0;
    }
    return -1;
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