#include <process.h>

/*t_process_in_mem new_process (uint32_t pid, char *path) {
    t_process_in_mem *process;
    process.pid = pid;
    process.path = path;
    return process;
}*/ //Si lo queres mas modularizado

void add_process (t_list *process_list, uint32_t pid, char *path) {
    t_process_in_mem *process;
    process->pid = pid;
    process->path = path;

    list_add(process_list, process);
    //free(process);
}

bool compare_pid (uint32_t pid, t_process_in_mem *process) {
    return pid == process->pid;
}

/*
t_process_in_mem *find_proc_through_pid (int pid, t_list *process_list) {
    return list_find(process_list, compare_pid());
}*/

t_process_in_mem *find_proc_through_pid (int pid, t_list *process_list) {
    t_link_element *element = process_list->head;
    while (!compare_pid(pid, element->data)){
        element = element->next;
    }
    return element->data;
}