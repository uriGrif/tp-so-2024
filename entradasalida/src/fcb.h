#ifndef FCB_H
#define FCB_H

#include <commons/collections/dictionary.h>
#include <commons/config.h>
#include <stdlib.h>
#include <utils.h>
#include <stdio.h>
#include <bitmap_monitor.h>
#include <dirent.h>
#include <bitmap_monitor.h>

#define METADATA_BLOQUE_INICIAL "BLOQUE_INICIAL"
#define METADATA_TAMANIO_ARCHIVO "TAMANIO_ARCHIVO"


typedef struct {
    int first_block;
    int size;
} t_fcb;

void init_fcbs(void);
void create_file(char* file_name);
void delete_file(char *file_name);
bool file_already_exists(char* file_name);

#endif