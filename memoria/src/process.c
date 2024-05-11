#include <process.h>

static t_list *process_list;

static pthread_mutex_t MUTEX_PROCESS_LIST;

void init_process_list (void) {
    process_list = list_create();
    pthread_mutex_init(&MUTEX_PROCESS_LIST, NULL);
}

t_process_in_mem *t_process_in_mem_create (void) {
    t_process_in_mem *process = malloc(sizeof(t_process_in_mem));
    return process;
} 

void t_process_in_mem_destroy (t_process_in_mem *process) {
    free(process->path);
    free(process);
} 

static void destroyer(void* process){
    t_process_in_mem* p = (t_process_in_mem *) process;
    t_process_in_mem_destroy(p);
}

void process_list_destroy (void) {
    list_destroy_and_destroy_elements(process_list, destroyer);
    pthread_mutex_destroy(&MUTEX_PROCESS_LIST);
}

void packet_get_process_in_mem (t_buffer *buffer, t_process_in_mem *process) {
    process->pid = packet_getUInt32(buffer);
    char* path = packet_getString(buffer);
    process->path = mount_instructions_directory(path);
    free(path);
}

void add_process (t_process_in_mem *process) {
    pthread_mutex_lock(&MUTEX_PROCESS_LIST);
    list_add(process_list, process);
    pthread_mutex_unlock(&MUTEX_PROCESS_LIST);
}

t_process_in_mem *find_process_by_pid (uint32_t pid) {
    bool closure (void *elem) {
        t_process_in_mem *aux_process = (t_process_in_mem*) elem;
        return aux_process->pid == pid;
    }
    pthread_mutex_lock(&MUTEX_PROCESS_LIST);
    t_process_in_mem *res = list_find(process_list, closure);
    pthread_mutex_unlock(&MUTEX_PROCESS_LIST);
    return res;
}

void remove_process_by_pid (uint32_t pid) {
    bool closure (void *elem) {
        t_process_in_mem *aux_process = (t_process_in_mem*) elem;
        return aux_process->pid == pid;
    }
    pthread_mutex_lock(&MUTEX_PROCESS_LIST);
    list_remove_and_destroy_by_condition(process_list, closure, destroyer);
    pthread_mutex_unlock(&MUTEX_PROCESS_LIST);
}