#ifndef CONTEXT_H
#define CONTEXT_H

#include<stdint.h>
#include<registers.h>

typedef struct {
    uint32_t pid;
    t_cpu_registers* registers;
    int quantum;
} t_exec_context;

extern t_exec_context context;


#endif