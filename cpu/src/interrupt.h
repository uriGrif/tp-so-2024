#ifndef COMM_INTR_H_
#define COMM_INTR_H_

#include <proto/proto.h>
#include <sockets/sockets.h>
#include<commons/log.h>
#include <semaphore.h>

typedef struct{
    int server_fd;
    int *dispatch_fd;
    t_log* logger;
} t_process_interrupt_args;


extern int interrupt_flag;

extern sem_t sem_interrupt;
extern sem_t sem_interrupt_done;

void handle_interrupt(void *void_args);


void init_sem_interrupt();
void close_sem_interrupt();

#endif