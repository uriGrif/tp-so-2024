#ifndef STATE_QUEUES_H
#define STATE_QUEUES_H

#include <sync_queue/sync_queue.h>
#include<pcb.h>

extern t_sync_queue *new_queue;
extern t_sync_queue *ready_queue;
extern t_sync_queue *blocked_queue;
extern t_sync_queue *exit_queue;

void init_queues(void);
void destroy_queues(void);

#endif