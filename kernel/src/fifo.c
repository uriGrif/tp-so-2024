#include <fifo.h>

t_pcb *ready_to_exec_fifo(void)
{
    t_pcb *pcb = queue_sync_pop(ready_queue);
    queue_sync_push(exec_queue, pcb);
    pcb->state = EXEC;
    send_context_to_cpu(pcb->context);
    return pcb;
}

void dispatch_fifo(t_pcb *pcb, t_log *logger)
{
    if (wait_for_dispatch_reason(pcb, logger) == -1)
    {
        log_error(logger, "error waiting for cpu context");
    }
    if(sync_queue_length(exec_queue)==1) queue_sync_pop(exec_queue);
}

void block_to_ready_fifo(char *resource, t_log *logger)
{
    t_sync_queue *q = get_blocked_queue_by_name(resource);
    //wait sem cola bloqueado
    t_pcb *pcb = queue_sync_pop(q);
    pcb->state = READY;
    log_info(logger, "PID: %d - Estado Anterior: BLOCKED - Estado Actual: READY", pcb->context->pid);
    queue_sync_push(ready_queue, pcb);
}