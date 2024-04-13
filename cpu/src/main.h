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

#define CONFIG_PATH "cpu.config"
#define LOG_LEVEL LOG_LEVEL_DEBUG
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

#endif