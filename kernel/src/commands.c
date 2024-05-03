#include <commands.h>

// TODO: ALGUNO DE ESTOS PROCESOS DEBERIAN TENER UN LOGGER DESPUES
//  PODEMOS PENSAR SI EXPORTAMOS EL GLOBAL O CREAMOS OTROS NUEVOS

static char *get_pids_of_blocked_queues(void);

int multiprogramming_controler = 0;

void init_process(char *path, t_log *logger)
{
    t_pcb *pcb = pcb_create(path);
    // send_create_process(pcb);
    queue_sync_push(new_queue, pcb);
    sem_post(&scheduler.sem_new);
    // print_ready_queue(logger);
    log_info(logger, "Se crea el proceso %d en NEW", pcb->context->pid);
}

void end_process(char *pid_str, t_log *logger)
{
    uint32_t pid = atoi(pid_str);
    // aca si hay que manjear el atoi
    if (!pid || (int)pid < 0)
    {
        log_error(logger, "Error: %s no es un pid valido", pid_str);
        return;
    }
    send_end_process(pid);
    log_info(logger, "Finaliza el proceso %d (por consola) ", pid);
}

void stop_scheduler(char *x, t_log *logger)
{
    pause_threads();
}

void start_scheduler(char *x, t_log *logger)
{
    resume_threads();
}

void multiprogramming(char *value, t_log *logger)
{
    int new_grade = atoi(value);
    if (!new_grade || new_grade < 0)
    {
        log_error(logger, "Error: %s no es un grado valido", value);
        return;
    }
    pthread_mutex_lock(&current_multiprogramming_grade_mutex);
    if (new_grade >= max_multiprogramming_grade)
    {
        for (int i = 0; i < new_grade - max_multiprogramming_grade; i++)
        {
            current_multiprogramming_sem_mirror++;
            sem_post(&current_multiprogramming_grade);
        }
    } else {
        if (current_multiprogramming_sem_mirror > new_grade) {
            // si ya es cero no pasa por este while, no se bloquea este hilo
            // y no me puede waitear el long term porque usa el mutex
            while(current_multiprogramming_sem_mirror > 0){
                sem_wait(&current_multiprogramming_grade);
                current_multiprogramming_sem_mirror--;
            }
        }
    }
    pthread_mutex_unlock(&current_multiprogramming_grade_mutex);
    pthread_mutex_lock(&max_multiprogramming_grade_mutex);
    max_multiprogramming_grade = new_grade;
    pthread_mutex_unlock(&max_multiprogramming_grade_mutex);
    log_info(logger, "voy a cambiar el grado de multiprogramacion a: %d\n", new_grade);
}

void list_processes_by_state(char *x, t_log *logger)
{
    // TODO
    // log_info(logger,"voy a listar todos los procesos por estado\n");
    char *pids = generate_string_of_pids(new_queue);
    log_info(logger, "Estado NEW: %s", pids);
    free(pids);
    pids = generate_string_of_pids(ready_queue);
    char * pids2 = generate_string_of_pids(ready_plus_queue);
    log_info(logger, "Estado READY: %s READY+ %s", pids, pids2);
    free(pids2);
    free(pids);
    pids = generate_string_of_pids(exec_queue);
    log_info(logger, "Estado EXEC: %s", pids);
    free(pids);
    pids = get_pids_of_blocked_queues();
    log_info(logger, "Estado BLOCKED: %s", pids);
    free(pids);
    pids = generate_string_of_pids(exit_queue);
    log_info(logger, "Estado EXIT: %s", pids);
    free(pids);
    // sync_queue_iterate(ready_queue, log_state);
    // sync_queue_iterate(exec_queue, log_state);
}

static char *get_pids_of_blocked_queues(void)
{
    char *pids = strdup("[");
    void gen_pids_one_queue(void *void_queue)
    {
        t_blocked_queue *queue = (t_blocked_queue *)void_queue;
        void add_pid(void *elem)
        {
            t_pcb *pcb = (t_pcb *)elem;
            string_append_with_format(&pids, "%d,", pcb->context->pid);
        }
        if (queue)
            sync_queue_iterate(queue->block_queue, add_pid);
    }
    blocked_queues_iterate(gen_pids_one_queue);
    if (strlen(pids) > 1)
        pids[strlen(pids) - 1] = ']';
    else
        string_append(&pids, "]");
    return pids;
}
