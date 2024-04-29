#ifndef STATE_QUEUES_H
#define STATE_QUEUES_H

#include <sync_queue/sync_queue.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include<semaphore.h>
#include<utils.h>
#include <pcb.h>

typedef struct
{
    char *resource_name;
    union
    {
        int fd;
        int instances;
    };
    sem_t sem_process_count;
    t_sync_queue *block_queue;
} t_blocked_queue;

extern t_sync_queue *new_queue;
extern t_sync_queue *ready_queue;
extern t_sync_queue *ready_plus_queue;
extern t_sync_queue *exec_queue;
extern t_sync_queue *exit_queue;
extern pthread_mutex_t MUTEX_LISTA_BLOCKEADOS;

void init_queues(void);
void destroy_queues(void);
void add_blocked_queue(char *resource_name, int value);
int blocked_queue_push(char* resource_name, void* elem);
void *blocked_queue_pop(char* resource_name);
t_blocked_queue *get_blocked_queue_by_fd(int fd);
void remove_blocked_queue_by_fd(int fd);
t_blocked_queue *get_blocked_queue_by_name(char *resource_name);
void blocked_queues_iterate(void (*iterator)(void *));
void print_ready_queue(t_log* logger);

#endif