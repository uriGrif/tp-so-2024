#include <instr_set.h>

void set(char **args);
void sum(char **args);
void sub(char **args);
void jnz(char **args);
void io_gen_sleep(char **args);
void mov_in(char **args);
void mov_out(char **args);
void resize(char **args);
void copy_string(char **args);
void wait(char **args);
void signal(char **args);
void io_stdin_read(char **args);
void io_stdout_write(char **args);
void io_fs_create(char **args);
void io_fs_delete(char **args);
void io_fs_truncate(char **args);
void io_fs_write(char **args);
void io_fs_read(char **args);
// para no tener conflitcto
void instruction_exit(char **args);

t_instruction INSTRUCTION_SET[] = {{"SET", set}, {"SUM", sum}, {"SUB", sub}, {"JNZ", jnz}, {"IO_GEN_SLEEP", io_gen_sleep}, {"MOV_IN", mov_in}, {"MOV_OUT", mov_out}, {"RESIZE", resize}, {"COPY_STRING", copy_string}, {"WAIT", wait}, {"SIGNAL", signal}, {"IO_STDIN_READ", io_stdin_read}, {"IO_STDOUT_WRITE", io_stdout_write}, {"IO_FS_CREATE", io_fs_create}, {"IO_FS_DELETE", io_fs_delete}, {"IO_FS_TRUNCATE", io_fs_truncate}, {"IO_FS_WRITE", io_fs_write}, {"IO_FS_READ", io_fs_read}, {"EXIT", instruction_exit}, {NULL, NULL}};

// CON ESTA NOMENCLATURA CADA INSTRUCCION DEBE DESERIALIZAR SUS PARAMETROSs

t_instruction *instruction_get_by_name(char *name)
{
    for (uint8_t i = 0; INSTRUCTION_SET[i].name != NULL; i++)
    {
        if (!strcmp(INSTRUCTION_SET[i].name, name))
            return &INSTRUCTION_SET[i];
    }
    return NULL;
}

void set(char **args)
{
    t_register *reg = register_get_by_name(args[0]);

    if (sizeof(uint8_t) == reg->size)
    {
        uint8_t value = (uint8_t)atoi(args[1]);
        memcpy(reg->address, &value, reg->size);
        return;
    }

    uint32_t value = (uint32_t)atoi(args[1]);
    memcpy(reg->address, &value, reg->size);
}

void sum(char **args)
{
    t_register *dest = register_get_by_name(args[0]);
    t_register *src = register_get_by_name(args[1]);

    if (sizeof(uint8_t) == dest->size)
    {
        uint8_t *src_value = (uint8_t *)src->address;
        uint8_t *dest_value = (uint8_t *)dest->address;

        *dest_value += *src_value;
        return;
    }

    uint32_t *src_value = (uint32_t *)src->address;
    uint32_t *dest_value = (uint32_t *)dest->address;

    *dest_value += *src_value;
}

void sub(char **args)
{
    t_register *dest = register_get_by_name(args[0]);
    t_register *src = register_get_by_name(args[1]);

    if (sizeof(uint8_t) == dest->size)
    {
        uint8_t *src_value = (uint8_t *)src->address;
        uint8_t *dest_value = (uint8_t *)dest->address;

        *dest_value -= *src_value;
        return;
    }

    uint32_t *src_value = (uint32_t *)src->address;
    uint32_t *dest_value = (uint32_t *)dest->address;

    *dest_value -= *src_value;
}

void jnz(char **args)
{
    t_register *reg = register_get_by_name(args[0]);
    if (sizeof(uint8_t) == reg->size)
    {
        uint8_t *value = (uint8_t *)reg->address;
        registers.pc = *value ? atoi(args[1]) : registers.pc;
        return;
    }

    uint32_t *value = (uint32_t *)reg->address;
    registers.pc = *value ? atoi(args[1]) : registers.pc;
}

void io_gen_sleep(char **args)
{
    // TODO
}

void mov_in(char **args){
    
}
void mov_out(char **args){

}
void resize(char **args){

}
void copy_string(char **args){

}
void wait(char **args){

}
void signal(char **args){

}
void io_stdin_read(char **args){

}
void io_stdout_write(char **args){

}
void io_fs_create(char **args){

}
void io_fs_delete(char **args){

}
void io_fs_truncate(char **args){

}

void io_fs_write(char **args){

}
void io_fs_read(char **args){

}
// para no tener conflitcto
void instruction_exit(char **args){

}
