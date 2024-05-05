#include <virtual_round_robin.h>

t_temporal *timer;
int time_elapsed;

t_pcb *ready_to_exec_vrr(void)
{
    t_pcb* pcb;
    if(sync_queue_length(ready_plus_queue) > 0){
        pcb = queue_sync_pop(ready_plus_queue);
        pcb->state = EXEC;
        queue_sync_push(exec_queue,pcb);
        send_context_to_cpu(pcb->context);
        return pcb;
    }
    return ready_to_exec_fifo();
}

void dispatch_vrr(t_pcb *pcb, t_log *logger)
{
    timer = temporal_create();
    dispatch_rr(pcb, logger);
    temporal_destroy(timer);
}

void exec_to_ready_vrr(t_pcb *pcb, t_log *logger)
{
    pcb->context->quantum = cfg_kernel->quantum;
    exec_to_ready_rr(pcb, logger);
}

int move_pcb_to_blocked_vrr(t_pcb *pcb, char *resource_name, t_log *logger)
{
    log_info(logger, "time elapsed: %d", time_elapsed);
    if (!is_resource(resource_name) && time_elapsed < pcb->context->quantum){
        pcb->context->quantum -= time_elapsed;
        log_debug(logger,"quantum actual %d",pcb->context->quantum);
        return move_pcb_to_blocked_fifo(pcb, resource_name, logger);
    }

    pcb->context->quantum = cfg_kernel->quantum;
    return move_pcb_to_blocked_fifo(pcb, resource_name, logger);
}

void block_to_ready_vrr(char *resource, t_log *logger)
{
    t_pcb *pcb = blocked_queue_pop(resource);
    if (!pcb){
        log_error(logger, "blocked queue not found");
        return;
    }
    pcb->state = READY;
    if(pcb->context->quantum < cfg_kernel->quantum){
        queue_sync_push(ready_plus_queue, pcb);
        log_info(logger, "PID: %d - Estado Anterior: BLOCKED - Estado Actual: READY PLUS", pcb->context->pid);
        return;
    }
    queue_sync_push(ready_queue, pcb);
    log_info(logger, "PID: %d - Estado Anterior: BLOCKED - Estado Actual: READY", pcb->context->pid);
}