#include <state_queues.h>

t_sync_queue *new_queue;
t_sync_queue *ready_queue;
t_sync_queue *ready_plus_queue;
t_sync_queue *exec_queue;
t_sync_queue *exit_queue;
static t_dictionary *blocked_queues;

static void destroy_blocked_queues(void);
void pcb_destroyer(void *elem);

void init_queues(void)
{
    new_queue = sync_queue_create();
    ready_queue = sync_queue_create();
    ready_plus_queue = sync_queue_create();
    exec_queue = sync_queue_create();
    exit_queue = sync_queue_create();
    blocked_queues = dictionary_create();
}

void destroy_queues(void)
{
    sync_queue_destroy_with_destroyer(new_queue, pcb_destroyer);
    sync_queue_destroy_with_destroyer(ready_queue, pcb_destroyer);
    sync_queue_destroy_with_destroyer(ready_plus_queue, pcb_destroyer);
    sync_queue_destroy_with_destroyer(exec_queue, pcb_destroyer);
    sync_queue_destroy_with_destroyer(exit_queue, pcb_destroyer);
    destroy_blocked_queues();
}

void pcb_destroyer(void *elem)
{
    t_pcb *pcb = (t_pcb *)elem;
    pcb_destroy(pcb);
}

void add_blocked_queue(char *resource_name)
{
    if (resource_name) {
        t_sync_queue *q = sync_queue_create();
        dictionary_put(blocked_queues, resource_name, q);
    }
}

t_sync_queue* get_blocked_queue(char *resource_name)
{
    return dictionary_get(blocked_queues,resource_name);
}

static void destroy_blocked_queues(void){
    void destroyer(void* queue){
        t_sync_queue* q = (t_sync_queue*) queue;
        sync_queue_destroy(q);
    }
    dictionary_destroy_and_destroy_elements(blocked_queues,destroyer);
}

void blocked_queues_iterate(void (*iterator)(void*)){ // iterator tiene que recibir una cola
    t_list* list = dictionary_elements(blocked_queues);
    // genrar string of pids por cada cola
    list_iterate(list,iterator);
    list_destroy(list);
}