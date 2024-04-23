#include <process.h>

static t_list *process_list;

void init_process_list (void) {
    process_list = list_create();
}

t_process_in_mem *t_process_in_mem_create (void) {
    t_process_in_mem *process = malloc(sizeof(t_process_in_mem));
    return process;
} 

void t_process_in_mem_destroy (t_process_in_mem *process) {
    free(process->path);
    free(process);
} 

void process_list_destroy (void) {
    list_destroy_and_destroy_elements(process_list, t_process_in_mem_destroy);
}

void packet_get_process_in_mem (t_buffer *buffer, t_process_in_mem *process) {
    process->pid = packet_getUInt32(buffer);
    process->path = packet_getString(buffer);
}

void add_process (t_process_in_mem *process) {
    list_add(process_list, process);
}

t_process_in_mem *find_process_by_pid (uint32_t pid) {
    bool closure (void *elem) {
        t_process_in_mem *aux_process = (t_process_in_mem*) elem;
        return aux_process->pid == pid;
    }

    return list_find(process_list, closure);
}

void remove_process_by_pid (uint32_t pid) {
    bool closure (void *elem) {
        t_process_in_mem *aux_process = (t_process_in_mem*) elem;
        return aux_process->pid == pid;
    }

    list_remove_and_destroy_by_condition(process_list, closure, t_process_in_mem_destroy);
}