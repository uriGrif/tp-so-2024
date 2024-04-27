#include <round_robin.h>

static t_temporal* timer;

t_pcb *ready_to_exec_rr(void)
{
    return ready_to_exec_fifo();
}

void quantum_interruption_handler(void *args)
{
    t_sync_queue *ex_queue = (t_sync_queue*) args;
    while (1)
    {
        sem_wait(&scheduler.sem_quantum_timer);
        timer = temporal_create();
        t_pcb *process_in_execution = sync_queue_peek(ex_queue, 0);  
        //sleep(msToSeconds(process_in_execution->context->quantum));
        do{
            if(IO_REQUESTED)
                break;
        } while(temporal_gettime(timer) < process_in_execution->context->quantum);
        if(!IO_REQUESTED)
            send_interrupt();
        temporal_destroy(timer);
        IO_REQUESTED = false;
    }
}

void dispatch_rr(t_pcb *pcb, t_log *logger)
{
    sem_post(&scheduler.sem_quantum_timer);

    if (wait_for_dispatch_reason(pcb, logger) == -1)
    {
        log_error(logger, "error waiting for cpu context");
    }
    queue_sync_pop(exec_queue);
}

void exec_to_ready_rr(t_pcb *pcb, t_log *logger)
{
    log_info(logger, "PID: %d - Estado Anterior: EXEC - Estado Actual: READY", pcb->context->pid);
    queue_sync_push(ready_queue,pcb);
    sem_post(&scheduler.sem_ready);
}

int move_pcb_to_blocked_rr(t_pcb *pcb, char *resource_name, t_log *logger)
{
    return move_pcb_to_blocked_fifo(pcb,resource_name,logger);
}

void block_to_ready_rr(char *resource, t_log *logger)
{
    // buscar la cola por nombre de recurso
    // popear el primero (EN DUDA)
    // pasar a ready
    block_to_ready_fifo(resource, logger);
}


// int move_pcb_to_blocked_vrr(t_pcb *pcb, char *resource_name, t_log *logger)
// {
//     temporal_stop(timer);
//     int ms_passed = temporal_gettime(timer);
//     pcb->context->quantum -= ms_passed;
//     move_pcb_to_blocked_fifo(pcb,resource_name,logger);
// }