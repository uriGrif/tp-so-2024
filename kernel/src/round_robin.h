#ifndef RR_H
#define RR_H

#include<dispatcher.h>
#include<interrupt_comms.h>
#include<pcb.h>
#include<state_queues.h>
#include<commons/log.h>
#include<fifo.h>

t_pcb* ready_to_exec_rr(void);

void dispatch_rr(t_pcb* pcb, t_log* logger);

#endif