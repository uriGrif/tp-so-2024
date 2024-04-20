#ifndef COMM_INTR_H_
#define COMM_INTR_H_

#include<proto/proto.h>
#include<sockets/sockets.h>
#include<commons/log.h>

extern int interrupt_flag;


void handle_interrupt(void *_args);

#endif