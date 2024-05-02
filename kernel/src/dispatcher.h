#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <interface.h>
#include <pcb.h>
#include <commons/log.h>
#include <scheduler.h>

extern int fd_dispatch;

void send_context_to_cpu(t_exec_context *context);

int wait_for_dispatch_reason(t_pcb *pcb, t_log *logger);


#endif