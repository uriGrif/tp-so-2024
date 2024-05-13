#include <commands.h>

// TODO: ALGUNO DE ESTOS PROCESOS DEBERIAN TENER UN LOGGER DESPUES
//  PODEMOS PENSAR SI EXPORTAMOS EL GLOBAL O CREAMOS OTROS NUEVOS

static bool paused_by_console = false;

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
     t_pcb* victim;
    // empiezo a buscar
    if(!paused_by_console)
        pause_threads();
    
    //primero me apuro para ver si esta ejecutando pero no lo saco, dejo que el se saque solo
    if((victim = find_pcb_by_pid(exec_queue,pid))){
        send_interrupt(INTERRUPT_EXEC);
        //KILL_SUCCESS = true; 
        // haciendo esto le aviso que lo quiero matar asi no me escapa a bloqueado o a exit
    }
    // busco en new
    else if((victim = remove_pcb_by_pid(new_queue,pid))){
        sem_wait(&scheduler.sem_new);
        // no sumo grado de multiprogramacion
        log_info(logger, "Finaliza el proceso %d- Motivo: ASESINADO POR CONSOLA", victim->context->pid);
        log_info(logger, "PID: %d - Estado Anterior: NEW - Estado Actual EXIT", victim->context->pid);
        queue_sync_push(exit_queue,victim);
    }
    // busco en ready o ready +
    else if((victim = remove_pcb_by_pid(ready_queue,pid)) || (victim = remove_pcb_by_pid(ready_plus_queue,pid))){
        sem_wait(&scheduler.sem_ready);
        log_info(logger, "Finaliza el proceso %d- Motivo: ASESINADO POR CONSOLA", victim->context->pid);
        move_pcb_to_exit(victim,logger);
    }
    //busco en bloqueados por si las dudas uso el mutex
    else if((victim = remove_pcb_from_blocked_queues_by_pid(pid))){
        log_info(logger, "Finaliza el proceso %d- Motivo: ASESINADO POR CONSOLA", victim->context->pid);
        move_pcb_to_exit(victim,logger);
    }
    else{
        log_info(logger,"El proceso %d no existe o ya habia finalizado",pid);
    }
    
    if(!paused_by_console)
        resume_threads();
}

void stop_scheduler(char *x, t_log *logger)
{
    paused_by_console = true;
    pause_threads();
}

void start_scheduler(char *x, t_log *logger)
{
    paused_by_console = false;
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
    change_multiprogramming(new_grade);
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
    char *pids2 = generate_string_of_pids(ready_plus_queue);
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

void list_resources(char *x, t_log *logger){
    print_resources(logger);
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
    pthread_mutex_lock(&MUTEX_LISTA_BLOCKEADOS);
    blocked_queues_iterate(gen_pids_one_queue);
    pthread_mutex_unlock(&MUTEX_LISTA_BLOCKEADOS);
    if (strlen(pids) > 1)
        pids[strlen(pids) - 1] = ']';
    else
        string_append(&pids, "]");
    return pids;
}
