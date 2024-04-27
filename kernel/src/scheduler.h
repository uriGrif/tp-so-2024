#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <sync_queue/sync_queue.h>
#include <state_queues.h>
#include <stdbool.h>
#include <commons/log.h>
#include <string.h>
#include <pcb.h>
#include <dispatcher.h>
#include <config.h>
#include <semaphore.h>
#include <memory_comms.h>
#include <fifo.h>
#include <round_robin.h>

typedef t_pcb *(*ready_to_exec_strategy)(void);
typedef void (*dispatch_strategy)(t_pcb *, t_log *);
typedef void (*block_to_ready_strategy)(char *resource,t_log* logger);
typedef void (*exec_to_ready_strategy)(t_pcb* pcb,t_log* logger);
typedef int (*move_pcb_to_blocked_strategy)(t_pcb *pcb, char *resource_name, t_log *logger);
// para vos escobar
typedef struct
{
    ready_to_exec_strategy ready_to_exec;
    dispatch_strategy dispatch;
    block_to_ready_strategy block_to_ready;
    exec_to_ready_strategy exec_to_ready;
    move_pcb_to_blocked_strategy move_pcb_to_blocked;
    sem_t sem_scheduler_paused;
    sem_t sem_ready;
    sem_t sem_new;
    sem_t sem_quantum_timer;
    // quizas alguna para manejar el exit y cerrar esta abstraccion
    // agregar los semaforos aca???
} t_scheduler;

extern t_scheduler scheduler;

extern bool scheduler_paused;

void init_scheduler(void);
void destroy_scheduler(void);

void handle_short_term_scheduler(void *args_logger);
void handle_long_term_scheduler(void *args_logger);

// int move_pcb_to_blocked(t_pcb* pcb, char* resource_name,t_log* logger);
void move_pcb_to_exit(t_pcb* pcb, t_log* logger);

#endif