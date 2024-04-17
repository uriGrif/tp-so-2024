#ifndef INSTR_SET_CPU_H
#define INSTR_SET_CPU_H

#include<registers.h>

typedef struct {
    char* name;
    void (*intr)(char** args);
} t_instruction;

extern const t_instruction INSTRUCTION_SET[];

#endif