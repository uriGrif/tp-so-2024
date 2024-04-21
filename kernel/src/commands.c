#include <commands.h>

// TODO: ALGUNO DE ESTOS PROCESOS DEBERIAN TENER UN LOGGER DESPUES
//  PODEMOS PENSAR SI EXPORTAMOS EL GLOBAL O CREAMOS OTROS NUEVOS

int multiprogramming_controler = 0;

void init_process(char *path)
{
    if(multiprogramming_controler == cfg_kernel->grado_multiprogramacion){
        printf("el grado de multiprogramacion no permite crear un nuevo proceso\n");
        return;
    }

    t_pcb *pcb = pcb_create();
    pcb->text_path = path;
    queue_sync_push(new_queue, pcb);
    multiprogramming_controler++;

    printf("cree el proceso con este path: %s\n", path);
}

void end_process(char *pid_str)
{
    int pid = atoi(pid_str);
    printf("terminado proceso con pid %d\n", pid);
}

void stop_scheduler(char *x)
{
    printf("se detuvo la planificacion\n");
}

void start_scheduler(char *x)
{
    printf("arranco la planificacion\n");
}

void multiprogramming(char *value)
{
    int new_grade = atoi(value);
    printf("voy a cambiar el grado de multiprogramacion a: %d\n", new_grade);
}
void list_processes_by_state(char *x)
{
    printf("voy a listar todos los procesos por estado\n");
}
