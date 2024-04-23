#include <commands.h>

// TODO: ALGUNO DE ESTOS PROCESOS DEBERIAN TENER UN LOGGER DESPUES
//  PODEMOS PENSAR SI EXPORTAMOS EL GLOBAL O CREAMOS OTROS NUEVOS

int multiprogramming_controler = 0;

void init_process(char *path, t_log* logger)
{
    // if(multiprogramming_controler == cfg_kernel->grado_multiprogramacion){
    //     printf("el grado de multiprogramacion no permite crear un nuevo proceso\n");
    //     return;
    // }

    t_pcb *pcb = pcb_create(path);
    //log_info(logger,"path %s",pcb->text_path);
    // queue_sync_push(new_queue, pcb);
    //pcb->state = NEW;
    send_create_process(pcb);
    queue_sync_push(ready_queue, pcb);
    sem_post(&sem_ready); // por ahora para probar
    //multiprogramming_controler++;

    log_info(logger,"Se crea el proceso %d en NEW", pcb->context->pid);
}

void end_process(char *pid_str, t_log* logger)
{
    uint32_t pid = atoi(pid_str);
    send_end_process(pid);
    //multiprogramming_controler--;
    log_info(logger,"Finaliza el proceso %d (por consola) ", pid);
}

void stop_scheduler(char *x, t_log* logger)
{
    scheduler_paused = true;
    log_info(logger,"se detuvo la planificacion\n");
}

void start_scheduler(char *x, t_log* logger)
{
    if(scheduler_paused)
        sem_post(&sem_scheduler_paused);
    log_info(logger,"arranco la planificacion\n");
}

void multiprogramming(char *value, t_log* logger)
{
    int new_grade = atoi(value);
    log_info(logger,"voy a cambiar el grado de multiprogramacion a: %d\n", new_grade);
}
void list_processes_by_state(char *x, t_log* logger)
{
    //TODO
    //log_info(logger,"voy a listar todos los procesos por estado\n");
    // void log_state(void* elem){
        
    // }
    // sync_queue_iterate(new_queue, log_state);
    // sync_queue_iterate(ready_queue, log_state);
    // sync_queue_iterate(exec_queue, log_state);
}
