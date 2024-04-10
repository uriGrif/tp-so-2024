#ifndef IO_H
#define IO_H

#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <sockets/sockets.h>
#include <proto/proto.h>
#include <errno.h>
#include <commons/log.h>
#include <commons/config.h>

#define CONFIG_PATH "IO.config"
#define LOG_LEVEL LOG_LEVEL_INFO
#define LOG_PATH "IO.log"
#define PROCESS_NAME "IO"

typedef struct
{
    char *tipo_interfaz;
    int unidad_trabajo;
    char *ip_kernel;
    char *puerto_kernel;
    char *ip_memoria;
    char *puerto_memoria;
    char *path_base_dialfs;
    int block_size;
    int block_count;
} t_io_config;

#endif