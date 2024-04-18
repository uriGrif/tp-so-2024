#include <instr_loop.h>

char *fetch(int fd_memoria)
{
    return NULL;
}

void decode_and_execute(char *instruction)
{
    char **tokens = string_split(instruction, " ");
    char *instr_name = tokens[0];
    char **args = tokens + 1;

    t_instruction *inst = instruction_get_by_name(instr_name);

    inst->instr(args);
    string_array_destroy(tokens);
}

void check_interrupt(t_queue* interruption_queue, int dispatch_fd){
    // TODO
}
