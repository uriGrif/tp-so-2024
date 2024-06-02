#include <instr_set.h>

int current_exec_process_has_finished = 0;

void set(char **args, t_log *logger);
void sum(char **args, t_log *logger);
void sub(char **args, t_log *logger);
void jnz(char **args, t_log *logger);
void io_gen_sleep(char **args, t_log *logger);
void mov_in(char **args, t_log *logger);
void mov_out(char **args, t_log *logger);
void resize(char **args, t_log *logger);
void copy_string(char **args, t_log *logger);
void wait_instr(char **args, t_log *logger);
void signal_instr(char **args, t_log *logger); // cambio el nombre por confilcto con el signal para el ctrl+C
void io_stdin_read(char **args, t_log *logger);
void io_stdout_write(char **args, t_log *logger);
void io_fs_create(char **args, t_log *logger);
void io_fs_delete(char **args, t_log *logger);
void io_fs_truncate(char **args, t_log *logger);
void io_fs_write(char **args, t_log *logger);
void io_fs_read(char **args, t_log *logger);
// para no tener conflitcto
void instruction_exit(char **args, t_log *logger);

static void send_io_gen_sleep(char *interface_name, int work_units);
enum StdType
{
    IN,
    OUT
};
static void send_io_std(enum StdType type, char *interface_name, t_list *access_list, uint32_t size);
static void send_wait_resource(char *resource_name);
static void send_signal_resource(char *resource_name);
static void send_resize(uint32_t size);

t_instruction INSTRUCTION_SET[] = {{"SET", set}, {"SUM", sum}, {"SUB", sub}, {"JNZ", jnz}, {"IO_GEN_SLEEP", io_gen_sleep}, {"MOV_IN", mov_in}, {"MOV_OUT", mov_out}, {"RESIZE", resize}, {"COPY_STRING", copy_string}, {"WAIT", wait_instr}, {"SIGNAL", signal_instr}, {"IO_STDIN_READ", io_stdin_read}, {"IO_STDOUT_WRITE", io_stdout_write}, {"IO_FS_CREATE", io_fs_create}, {"IO_FS_DELETE", io_fs_delete}, {"IO_FS_TRUNCATE", io_fs_truncate}, {"IO_FS_WRITE", io_fs_write}, {"IO_FS_READ", io_fs_read}, {"EXIT", instruction_exit}, {NULL, NULL}};

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

void set(char **args, t_log *logger)
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

void sum(char **args, t_log *logger)
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

void sub(char **args, t_log *logger)
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

void jnz(char **args, t_log *logger)
{
    uint32_t *pc = &context.registers.pc;
    t_register *reg = register_get_by_name(args[0]);
    if (sizeof(uint8_t) == reg->size)
    {
        uint8_t *value = (uint8_t *)reg->address;
        *pc = *value ? atoi(args[1]) : *pc;
        return;
    }

    uint32_t *value = (uint32_t *)reg->address;
    *pc = *value ? atoi(args[1]) : *pc;
}

void mov_in(char **args, t_log *logger)
{
    t_register *data = register_get_by_name(args[0]);
    t_register *dir = register_get_by_name(args[1]);

    uint32_t *dir_value = (uint32_t *)dir->address;

    t_list *access_to_memory = access_to_memory_create(*dir_value, data->size, PAGE_SIZE, logger);

    memory_send_read(fd_memory, context.pid, access_to_memory, data->size);
    t_packet *packet = packet_new(-1);
    if (packet_recv(fd_memory, packet) == -1)
    {
        log_error(logger, "Error, desconexion de memoria");
        packet_free(packet);
        return;
    }

    if(packet->op_code != READ_MEM_OK){
        log_info(logger,"Error al leer de memoria");
        packet_free(packet);
        return;
    }

    t_memory_read_ok_msg *ok_msg = malloc(sizeof(t_memory_read_ok_msg));
    memory_decode_read_ok(packet->buffer, ok_msg, data->size);
    memcpy(data->address, ok_msg->value, data->size);

    int offset = 0;
    void iterator(void *elem)
    {
        t_access_to_memory *access = (t_access_to_memory *)elem;
        void *temp = calloc(sizeof(int), sizeof(char));
        memcpy(temp, ok_msg->value + offset, access->bytes_to_access);
        offset += access->bytes_to_access;
        log_info(logger, "PID: %d - Accion: LEER - Direccion Fisica: %d - Valor: %d", context.pid, access->address, *(int *)temp);
        free(temp);
    }
    // aiuda
    list_iterate(access_to_memory, iterator);

    list_destroy_and_destroy_elements(access_to_memory, free);

    memory_destroy_read_ok(ok_msg);
    packet_free(packet);
}

void mov_out(char **args, t_log *logger)
{
    t_register *dir = register_get_by_name(args[0]);
    t_register *data = register_get_by_name(args[1]);

    uint32_t *dir_value = (uint32_t *)dir->address;

    t_list *access_to_memory = access_to_memory_create(*dir_value, data->size, PAGE_SIZE, logger);

    int offset = 0;
    void iterator(void *elem)
    {
        t_access_to_memory *access = (t_access_to_memory *)elem;
        void *temp = calloc(sizeof(int), sizeof(char));
        memcpy(temp, data->address+offset, access->bytes_to_access);
        offset += access->bytes_to_access;
        log_info(logger, "PID: %d - Accion: ESCRIBIR - Direccion Fisica: %d - Valor: %d", context.pid, access->address, *(int *)temp);
        free(temp);
    }

    list_iterate(access_to_memory,iterator);

    memory_send_write(fd_memory, context.pid, access_to_memory, data->size, data->address);

    list_destroy_and_destroy_elements(access_to_memory,free);

    t_packet* packet = packet_new(-1);
    if (packet_recv(fd_memory, packet) == -1)
    {
        log_error(logger, "Error, se desconecto la memoria");
        packet_free(packet);
        exit(1);
    }

    if(packet->op_code != WRITE_MEM_OK){
        log_info(logger,"Error al escribir en memoria");
        //hacer algo?? no se 
        packet_free(packet);
        return;
    }

    packet_free(packet);
}

void resize(char **args, t_log *logger)
{
    uint32_t size = (uint32_t)atoi(args[0]);
    send_resize(size);
    t_packet *packet = packet_new(-1);
    if (packet_recv(fd_memory, packet) == -1)
    {
        log_error(logger, "error al recibir de memoria");
        packet_free(packet);
        exit(1);
    }
    if (packet->op_code == OUT_OF_MEMORY)
    {
        send_dispatch_reason(OUT_OF_MEMORY, &context);
        wait_for_context(&context);
        clear_interrupt();
    }
    packet_free(packet);
}

void copy_string(char **args, t_log *logger)
{
    uint32_t size = (uint32_t)atoi(args[0]);
    t_register *source = register_get_by_name("SI");
    t_register *dest = register_get_by_name("DI");

    /*
    uint32_t *si_addr_ptr = (uint32_t *)source->address;
    t_physical_address *source_addr = translate_address_4_bytes(*si_addr_ptr, PAGE_SIZE);
    uint32_t *di_addr_ptr = (uint32_t *)dest->address;
    t_physical_address *dest_addr = translate_address_4_bytes(*di_addr_ptr, PAGE_SIZE);

    void *buffer = malloc(size);

    memory_send_read(fd_memory, context.pid, source_addr->page_number, source_addr->offset, size);

    t_packet *packet = packet_new(-1);
    if (packet_recv(fd_memory, packet) == -1)
    {
        log_error(logger, "error al leer de la memoria");
        packet_free(packet);
        return;
    }
    t_memory_read_ok_msg *ok_msg = malloc(sizeof(t_memory_read_ok_msg));
    memory_decode_read_ok(packet->buffer, ok_msg, size);
    memcpy(buffer, ok_msg->value, size);
    packet_free(packet);
    // No se si aca hace falta poner el log de que lei de memoria

    memory_send_write(fd_memory, context.pid, dest_addr->page_number, dest_addr->offset, size, buffer);

    packet = packet_new(-1);
    if (packet_recv(fd_memory, packet) == -1)
    {
        log_error(logger, "error al leer de la memoria");
        packet_free(packet);
        return;
    }

    // IDEM, no se si poner los logs

    free(source_addr);
    free(dest_addr);

    packet_free(packet);
    */
}

void wait_instr(char **args, t_log *logger)
{
    // clear_interrupt();
    char *resource_name = args[0];
    int last_pid = context.pid;
    send_wait_resource(resource_name);
    wait_for_context(&context);
    if (context.pid != last_pid)
        clear_interrupt();
    log_debug(logger, "me llego: pid: %d, quantum: %d, AX: %u", context.pid, context.quantum, context.registers.ax);
}
void signal_instr(char **args, t_log *logger)
{
    char *resource_name = args[0];
    send_signal_resource(resource_name);
    wait_for_context(&context);
    // log_debug(logger, "me llego: pid: %d, quantum: %d", context.pid, context.quantum);
}

void io_gen_sleep(char **args, t_log *logger)
{
    char *interface_name = args[0];
    uint32_t work_units = atoi(args[1]);
    send_io_gen_sleep(interface_name, work_units);
    wait_for_context(&context);
    clear_interrupt();
    log_debug(logger, "me llego: pid: %d, quantum: %d, AX: %u", context.pid, context.quantum, context.registers.ax);
}

void io_stdin_read(char **args, t_log *logger)
{
    char *interface_name = args[0];
    t_register *virtual_address = register_get_by_name(args[1]);
    uint32_t *size_dir = (uint32_t *)register_get_by_name(args[2])->address;
    /*
    if (sizeof(uint8_t) == virtual_address->size)
    {
        uint8_t *value = (uint8_t *)virtual_address->address;
        t_physical_address *physical_mem_dir = translate_address_1_byte(*value, PAGE_SIZE);
        send_io_std(IN, interface_name, physical_mem_dir->page_number, physical_mem_dir->offset, *size_dir);
        free(physical_mem_dir);
        wait_for_context(&context);
        clear_interrupt();
        log_debug(logger, "me llego: pid: %d, quantum: %d, AX: %u", context.pid, context.quantum, context.registers.ax);
        return;
    }
    uint32_t *value = (uint32_t *)virtual_address->address;
    t_physical_address *physical_mem_dir = translate_address_4_bytes(*value, PAGE_SIZE);

    send_io_std(IN, interface_name, physical_mem_dir->page_number, physical_mem_dir->offset, *size_dir);
    free(physical_mem_dir);
    wait_for_context(&context);
    clear_interrupt();
    log_debug(logger, "me llego: pid: %d, quantum: %d, AX: %u", context.pid, context.quantum, context.registers.ax);
    */
}

void io_stdout_write(char **args, t_log *logger)
{
    char *interface_name = args[0];
    t_register *virtual_address = register_get_by_name(args[1]);
    uint32_t *size_dir = (uint32_t *)register_get_by_name(args[2])->address;

    uint32_t *dir_value = (uint32_t *)virtual_address->address;

    t_list *access_to_memory = access_to_memory_create(*dir_value, *size_dir, PAGE_SIZE, logger);

    send_io_std(OUT, interface_name, access_to_memory, *size_dir);
    list_destroy_and_destroy_elements(access_to_memory, free);
    wait_for_context(&context);
    clear_interrupt();
    log_debug(logger, "me llego: pid: %d, quantum: %d, AX: %u", context.pid, context.quantum, context.registers.ax);
}

void io_fs_create(char **args, t_log *logger)
{
}
void io_fs_delete(char **args, t_log *logger)
{
}
void io_fs_truncate(char **args, t_log *logger)
{
}

void io_fs_write(char **args, t_log *logger)
{
}
void io_fs_read(char **args, t_log *logger)
{
}
// para no tener conflitcto
void instruction_exit(char **args, t_log *logger)
{
    clear_interrupt();
    send_dispatch_reason(END_PROCESS, &context);
    current_exec_process_has_finished = 1;
}

// MEM PROTOCOL

static void send_resize(uint32_t size)
{
    t_packet *packet = packet_new(RESIZE_PROCESS);
    packet_addUInt32(packet, context.pid);
    packet_addUInt32(packet, size);
    packet_send(packet, fd_memory);
    packet_free(packet);
}

// WAIT and SIGNAL protocol
static void send_wait_resource(char *resource_name)
{
    t_packet *packet = packet_new(WAIT);
    packet_add_context(packet, &context);
    packet_addString(packet, resource_name);
    packet_send(packet, cli_dispatch_fd);
    packet_free(packet);
}

static void send_signal_resource(char *resource_name)
{
    t_packet *packet = packet_new(SIGNAL);
    packet_add_context(packet, &context);
    packet_addString(packet, resource_name);
    packet_send(packet, cli_dispatch_fd);
    packet_free(packet);
}

// IO protocol
static void send_io_gen_sleep(char *interface_name, int work_units)
{
    t_packet *packet = packet_new(IO_GEN_SLEEP);
    packet_add_context(packet, &context);
    packet_addString(packet, interface_name);
    packet_addUInt32(packet, work_units);
    packet_send(packet, cli_dispatch_fd);
    packet_free(packet);
}

static void send_io_std(enum StdType type, char *interface_name, t_list *access_list, uint32_t size)
{
    t_packet *packet = packet_new(type == IN ? IO_STDIN_READ : IO_STDOUT_WRITE);
    packet_add_context(packet, &context);
    packet_addString(packet, interface_name);
    packet_add_list(packet, access_list, (void *)packet_add_access_to_mem);
    packet_addUInt32(packet, size);
    packet_send(packet, cli_dispatch_fd);
    packet_free(packet);
}
