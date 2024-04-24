#include <state_queues.h>

t_sync_queue *new_queue;
t_sync_queue *ready_queue;
t_sync_queue *ready_plus_queue;
t_sync_queue *exec_queue;
t_sync_queue *exit_queue;
static t_list *_blocked_queues;

static void destroy_blocked_queues(void);
void pcb_destroyer(void *elem);

void init_queues(void)
{
    new_queue = sync_queue_create();
    ready_queue = sync_queue_create();
    ready_plus_queue = sync_queue_create();
    exec_queue = sync_queue_create();
    exit_queue = sync_queue_create();
    _blocked_queues = list_create();
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

t_blocked_queue *blocked_queue_create(char *name, int value)
{
    t_blocked_queue *q = malloc(sizeof(t_blocked_queue));
    q->resource_name = strdup(name);
    q->fd = value;
    q->block_queue = sync_queue_create();
    return q;
}

void blocked_queue_destroy(t_blocked_queue *q)
{
    free(q->resource_name);
    sync_queue_destroy_with_destroyer(q->block_queue,pcb_destroyer);
    free(q);
}

void pcb_destroyer(void *elem)
{
    t_pcb *pcb = (t_pcb *)elem;
    pcb_destroy(pcb);
}

void add_blocked_queue(char *resource_name, int value)
{
    if (resource_name)
    {
        t_blocked_queue *q = blocked_queue_create(resource_name, value);
        list_add(_blocked_queues, q);
    }
}

t_sync_queue *get_blocked_queue_by_name(char *resource_name)
{
    bool closure(void* elem){
        t_blocked_queue* q = (t_blocked_queue*) elem;
        return !strcmp(q->resource_name,resource_name);
    }

    t_blocked_queue* tmp =  list_find(_blocked_queues, closure);
    if(tmp)
        return tmp->block_queue;
    return NULL;
}

t_sync_queue *get_blocked_queue_by_fd(int fd)
{
    bool closure(void* elem){
        t_blocked_queue* q = (t_blocked_queue*) elem;
        return fd == q->fd;
    }

    t_blocked_queue* tmp =  list_find(_blocked_queues, closure);
    if(tmp)
        return tmp->block_queue;
    return NULL;
}

static void destroy_blocked_queues(void)
{
    void destroyer(void *queue)
    {
        t_blocked_queue *q = (t_blocked_queue *)queue;
        blocked_queue_destroy(q);
    }
    list_destroy_and_destroy_elements(_blocked_queues, destroyer);
}

void remove_blocked_queue_by_fd(int fd){
     bool closure(void* elem){
        t_blocked_queue* q = (t_blocked_queue*) elem;
        return fd == q->fd;
    }
    void destroyer(void *queue)
    {
        t_blocked_queue *q = (t_blocked_queue *)queue;
        blocked_queue_destroy(q);
    }
    list_remove_and_destroy_by_condition(_blocked_queues,closure,destroyer);
}

void blocked_queues_iterate(void (*iterator)(void *))
{ // iterator tiene que recibir una cola
    list_iterate(_blocked_queues, iterator);
}