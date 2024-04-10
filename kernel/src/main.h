#ifndef KERNEL_H
#define KERNEL_H

#include <stdlib.h>
#include <stdio.h>
#include <sockets/sockets.h>
#include <string.h>
#include <errno.h>
#include <proto/proto.h>
#include <commons/log.h>
#include <commons/config.h>
#include<commons/string.h>
#include <sys/signal.h>
#include<string.h>
#include <communications.h>

#define CONFIG_PATH "kernel.config"
#define LOG_LEVEL LOG_LEVEL_INFO
#define LOG_PATH "kernel.log"
#define PROCESS_NAME "Kernel"

typedef struct
{
    char *puerto_escucha;
    char *ip_memoria;
    char *puerto_memoria;
    char *ip_cpu;
    char *puerto_cpu_dispatch;
    char *puerto_cpu_interrupt;
    char *algoritmo_planificacion;
    int quantum;
    char **recursos;
    char **instancias_recursos;
    int grado_multiprogramacion;
} t_kernel_config;

#endif