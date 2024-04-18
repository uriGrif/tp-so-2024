#ifndef INSTR_CYCLE_CPU_H
#define INSTR_CYCLE_CPU_H

#include <instr_set.h>
#include<commons/collections/queue.h>
#include<registers.h>
#include<commons/string.h>
#include<stdio.h>
#include<commons/log.h>
#include<utils/utlis.h>
#include<proto/proto.h>

char *fetch(int fd_memoria,uint32_t pid,t_log* logger);

void decode_and_execute(char *instruction,uint32_t pid,t_log* logger);

//void execute(t_instruction* instruction);

void check_interrupt(t_queue* interruption_queue, int dispatch_fd);

#endif