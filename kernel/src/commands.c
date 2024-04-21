#include <commands.h>

// TODO: ALGUNO DE ESTOS PROCESOS DEBERIAN TENER UN LOGGER DESPUES
//  PODEMOS PENSAR SI EXPORTAMOS EL GLOBAL O CREAMOS OTROS NUEVOS

int multiprogramming_controler = 0;

void init_process(char *path, t_log* logger)
{
    if(multiprogramming_controler == cfg_kernel->grado_multiprogramacion){
        printf("el grado de multiprogramacion no permite crear un nuevo proceso\n");
        return;
    }

    t_pcb *pcb = pcb_create(path);
    log_info(logger,"path %s",pcb->text_path);
    queue_sync_push(new_queue, pcb);
    multiprogramming_controler++;
    send_create_process(pcb);

    log_info(logger,"Se crea el proceso %d en NEW", pcb->context->pid);
}

void end_process(char *pid_str, t_log* logger)
{
    uint32_t pid = atoi(pid_str);
    send_end_process(pid);
    multiprogramming_controler--;
    log_info(logger,"Finaliza el proceso %d (por consola) ", pid);
}

void stop_scheduler(char *x, t_log* logger)
{
    printf("se detuvo la planificacion\n");
}

void start_scheduler(char *x, t_log* logger)
{
    printf("arranco la planificacion\n");
}

void multiprogramming(char *value, t_log* logger)
{
    int new_grade = atoi(value);
    printf("voy a cambiar el grado de multiprogramacion a: %d\n", new_grade);
}
void list_processes_by_state(char *x, t_log* logger)
{
    printf("voy a listar todos los procesos por estado\n");
}
