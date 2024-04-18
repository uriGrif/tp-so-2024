#ifndef REGISTERS_H
#define REGISTERS_H

#include<stdint.h>
#include<stdlib.h>
#include<string.h>

typedef struct {
    uint32_t pc;
    uint8_t ax;
    uint8_t bx;
    uint8_t cx;
    uint8_t dx;
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t si;
    uint32_t di;
} t_cpu_registers;

typedef struct{
    char* name;
    void* address;
    size_t size;
} t_register;


extern t_cpu_registers registers;

extern t_register REGISTERS[];
t_register *register_get_by_name(char *name);

#endif