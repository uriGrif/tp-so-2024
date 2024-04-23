#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <sync_queue/sync_queue.h>
#include <state_queues.h>
#include <commons/log.h>
#include <pcb.h>
#include <dispatcher.h>
#include <semaphore.h>

extern bool scheduler_paused;
extern sem_t sem_scheduler_paused;

t_pcb *ready_to_exec(void);
// void exec_to_blocked(void);
// void blocked_to_ready(void);

void handle_short_term_scheduler(void* args_logger);
void handle_long_term_scheduler(void* args_logger);


#endif