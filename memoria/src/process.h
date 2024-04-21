#ifndef PROCESS
#define PROCESS

#include <proto.h>
#include <utils.h>

typedef struct{
    int pid;
    char* path;
} t_process_in_mem;

//t_process_in_mem new_process (uint32_t pid, char *path);
void add_process (t_list *process_list, uint32_t pid, char *path);
bool compare_pid (uint32_t pid, t_process_in_mem *process);
t_process_in_mem *find_proc_through_pid (int pid, t_list *process_list);
#endif