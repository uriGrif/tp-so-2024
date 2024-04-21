#ifndef IO_CONFIG_H
#define IO_CONFIG_H

typedef struct
{
    char *name;
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