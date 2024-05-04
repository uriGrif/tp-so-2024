#ifndef MEM_COMMS_CPU
#define MEM_COMMS_CPU

#include <proto/proto.h>
#include<commons/log.h>
#include<stdlib.h>

extern int fd_memory;

extern uint32_t PAGE_SIZE;

void send_mem_handshake(t_log* logger);

#endif