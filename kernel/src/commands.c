#include <commands.h>

// TODO: ALGUNO DE ESTOS PROCESOS DEBERIAN TENER UN LOGGER DESPUES
//  PODEMOS PENSAR SI EXPORTAMOS EL GLOBAL O CREAMOS OTROS NUEVOS

static char *generate_string_of_pids(t_sync_queue *queue);
static char *get_pids_of_blocked_queues(void);

int multiprogramming_controler = 0;

void init_process(char *path, t_log *logger)
{
    // if(multiprogramming_controler == cfg_kernel->grado_multiprogramacion){
    //     printf("el grado de multiprogramacion no permite crear un nuevo proceso\n");
    //     return;
    // }

    t_pcb *pcb = pcb_create(path);
    // log_info(logger,"path %s",pcb->text_path);
    //  queue_sync_push(new_queue, pcb);
    // pcb->state = NEW;
    send_create_process(pcb);
    queue_sync_push(ready_queue, pcb);
    sem_post(&scheduler.sem_ready); // por ahora para probar
    // multiprogramming_controler++;

    log_info(logger, "Se crea el proceso %d en NEW", pcb->context->pid);
}

void end_process(char *pid_str, t_log *logger)
{
    uint32_t pid = atoi(pid_str);
    send_end_process(pid);
    // multiprogramming_controler--;
    log_info(logger, "Finaliza el proceso %d (por consola) ", pid);
}

void stop_scheduler(char *x, t_log *logger)
{
    scheduler_paused = true;
    log_info(logger, "se detuvo la planificacion\n");
}

void start_scheduler(char *x, t_log *logger)
{
    if (scheduler_paused)
    {
        sem_post(&scheduler.sem_scheduler_paused);
        scheduler_paused = false;
    }
    log_info(logger, "arranco la planificacion\n");
}

void multiprogramming(char *value, t_log *logger)
{
    int new_grade = atoi(value);
    log_info(logger, "voy a cambiar el grado de multiprogramacion a: %d\n", new_grade);
}
void list_processes_by_state(char *x, t_log *logger)
{
    // TODO
    // log_info(logger,"voy a listar todos los procesos por estado\n");

    char *pids = generate_string_of_pids(ready_queue);
    log_info(logger, "Estado READY: %s", pids);
    free(pids);
    pids = get_pids_of_blocked_queues();
    log_info(logger, "Estado BLOCKED: %s", pids);
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

static char *generate_string_of_pids(t_sync_queue *queue)
{
    char *pids = strdup("[");
    void add_pid(void *elem)
    {
        t_pcb *pcb = (t_pcb *)elem;
        string_append_with_format(&pids, "%d,", pcb->context->pid);
    }
    sync_queue_iterate(ready_queue, add_pid);
    if (strlen(pids) > 1)
        pids[strlen(pids) - 1] = ']';
    else
        string_append(&pids, "]");
    return pids;
}
