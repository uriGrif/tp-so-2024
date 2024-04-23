#ifndef PROCESS_IN_MEM
#define PROCESS_IN_MEM

#include <stdint.h>
#include <utils.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <proto/proto.h>

typedef struct{
    int pid;
    char* path;
} t_process_in_mem;

void init_process_list (void);
t_process_in_mem *t_process_in_mem_create (void);
void process_list_destroy (void);
void packet_get_process_in_mem (t_buffer *buffer, t_process_in_mem *process);
void add_process (t_process_in_mem *process);
t_process_in_mem *find_process_by_pid (uint32_t pid);
void remove_process_by_pid (uint32_t pid);
#endif