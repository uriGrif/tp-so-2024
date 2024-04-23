#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <sync_queue/sync_queue.h>
#include <state_queues.h>
#include <commons/log.h>
#include<string.h>
#include <pcb.h>
#include <dispatcher.h>
#include<config.h>
#include <semaphore.h>
#include<fifo.h>
#include<round_robin.h>

typedef t_pcb* (*ready_to_exec_strategy)(void);
typedef void (*dispatch_strategy)(t_pcb*,t_log*);
// para vos escobar
typedef struct{
    ready_to_exec_strategy ready_to_exec; 
    dispatch_strategy dispatch;
    // quizas alguna para manejar el exit y cerrar esta abstraccion
} t_scheduler;

extern t_scheduler scheduler;

extern bool scheduler_paused;
extern sem_t sem_scheduler_paused;

t_pcb *ready_to_exec(void);
// void exec_to_blocked(void);
// void blocked_to_ready(void);

void handle_short_term_scheduler(void* args_logger);
void handle_long_term_scheduler(void* args_logger);


#endif