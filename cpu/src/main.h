#ifndef CPU_H
#define CPU_H

#include <stdlib.h>
#include <stdio.h>
#include <sockets/sockets.h>
#include <proto/proto.h>
#include <errno.h>
#include <commons/log.h>
#include <sys/signal.h>
#include <commons/config.h>
#include <string.h>
#include <interrupt.h>
#include <dispatch.h>
#include <instr_cycle.h>
#include <commons/collections>

#define CONFIG_PATH "cpu.config"
#define LOG_LEVEL LOG_LEVEL_INFO
#define LOG_PATH "cpu.log"
#define PROCESS_NAME "CPU"

typedef struct
{
    char *ip_memoria;
    char *puerto_memoria;
    char *puerto_escucha_dispatch;
    char *puerto_escucha_interrupt;
    int cantidad_entradas_tlb;
    char *algoritmo_tlb;
} t_cpu_config;

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
} t_registers;

#endif