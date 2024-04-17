#ifndef INSTR_CYCLE_CPU_H
#define INSTR_CYCLE_CPU_H

#include <instr_set.h>

char *fetch(int fd_memoria, uint32_t logical_address);

t_instruction decode(char *instruction, t_registers* registers);

void execute(t_instruction* instruction);

void check_interrupt(t_queue* interruption_queue, int dispatch_fd);

#endif