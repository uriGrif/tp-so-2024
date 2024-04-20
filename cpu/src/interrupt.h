#ifndef COMM_INTR_H_
#define COMM_INTR_H_

#include <proto/proto.h>
#include <sockets/sockets.h>
#include <commons/log.h>
#include <dispatch.h>
#include <global_context.h>
#include <semaphore.h>

extern int interrupt_flag;

extern sem_t sem_interrupt;
extern sem_t sem_interrupt_done;

void handle_interrupt(void *void_args);

void init_sem_interrupt();
void close_sem_interrupt();

#endif