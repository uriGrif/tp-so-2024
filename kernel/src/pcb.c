#include <pcb.h>

uint32_t get_next_pid(void);
void init_context(t_exec_context *context);

t_pcb *pcb_create(char* path)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));
    pcb->state = NEW;
    pcb->context = malloc(sizeof(t_exec_context));
    pcb->context->pid = get_next_pid();
    init_context(pcb->context);
    // asumo que si es por fifo me da cero y listo lo ignoro
    pcb->context->quantum = cfg_kernel->quantum;
    pcb->text_path = strdup(path);
    return pcb;
}

void pcb_destroy(t_pcb *pcb)
{
    free(pcb->context);
    free(pcb->text_path);
    free(pcb);
}

void init_context(t_exec_context *context)
{
    t_cpu_registers reg = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    context->registers = reg;
}

uint32_t get_next_pid(void)
{
    static uint32_t pid_counter =0;
    return ++pid_counter;
}