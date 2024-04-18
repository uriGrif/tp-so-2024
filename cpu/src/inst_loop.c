#include <instr_loop.h>

static char* args_as_string(char** args);

char *fetch(int fd_memoria,uint32_t pid,t_log* logger)
{
    log_info(logger,"PID: %d - FETCH - Program Counter: %d",pid,registers.pc);
    t_packet* request = packet_new(NEXT_INSTRUCTION);
    packet_addUInt32(request,pid);
    packet_addUInt32(request,registers.pc);
    packet_send(request,fd_memoria);
    packet_free(request);
    request = packet_new(-1);
    if(packet_recv(fd_memoria,request) == -1){
        printf("no me llego nada\n");
        packet_free(request);
        return NULL;
    }
    if(request->op_code != NEXT_INSTRUCTION){
        printf("opcode invaliido\n");
        packet_free(request);
        return NULL;
    }


    char* aux = packet_getString(request->buffer);
    packet_free(request);
    return aux;

}

void decode_and_execute(char *instruction,uint32_t pid,t_log* logger)
{
    char **tokens = string_split(instruction, " ");
    char *instr_name = tokens[0];
    char **args = tokens + 1;
    char * string_of_args = args_as_string(args);
    t_instruction *inst = instruction_get_by_name(instr_name);
    log_info(logger,"PID: %d - Ejecutando: %s - %s",pid,instr_name,string_of_args);
    free(string_of_args);
    inst->instr(args);
    string_array_destroy(tokens);
    free(instruction);
}

void check_interrupt(t_queue* interruption_queue, int dispatch_fd){
    // TODO
}


static char* args_as_string(char** args){
    char* result = string_new();
    void iterator(char* elem){
        string_append_with_format(&result,"%s ",elem);
    }
    string_iterate_lines(args,iterator);
    string_trim_right(&result);
    return result;

}