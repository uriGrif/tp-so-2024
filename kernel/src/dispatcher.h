#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <pcb.h>
#include <proto/proto.h>
#include <commons/log.h>
#include <scheduler.h>
#include <interface.h>

extern int fd_dispatch;

void send_context_to_cpu(t_exec_context *context);

int wait_for_dispatch_reason(t_pcb *pcb, t_log *logger);

// para los casos FINALIZAR PROCESO, ERROR Y FIN DE QUANTUM
int wait_for_context_no_reason(t_pcb *pcb);

#endif