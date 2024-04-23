#include <state_queues.h>

t_sync_queue *new_queue;
t_sync_queue *ready_queue;
t_sync_queue *ready_plus_queue;
t_sync_queue *exec_queue;
t_sync_queue *blocked_queue;
t_sync_queue *exit_queue;

void pcb_destroyer(void* elem);

void init_queues(void)
{
    new_queue = sync_queue_create();
    ready_queue = sync_queue_create();
    ready_plus_queue = sync_queue_create();
    exec_queue = sync_queue_create();
    exit_queue = sync_queue_create();
}

void destroy_queues(void)
{
    sync_queue_destroy_with_destroyer(new_queue, pcb_destroyer);
    sync_queue_destroy_with_destroyer(ready_queue, pcb_destroyer);
    sync_queue_destroy_with_destroyer(ready_plus_queue, pcb_destroyer);
    sync_queue_destroy_with_destroyer(exec_queue, pcb_destroyer);
    sync_queue_destroy_with_destroyer(exit_queue, pcb_destroyer);
}

void pcb_destroyer(void* elem){
    t_pcb* pcb = (t_pcb* ) elem;
    pcb_destroy(pcb);
}