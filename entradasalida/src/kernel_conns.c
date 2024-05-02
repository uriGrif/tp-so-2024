#include <kernel_conns.h>

static char *io_op_to_string(t_opcode operation);

int registerResourceInKernel(int kernel_fd, t_log *logger, t_io_config *cfg_io)
{
    log_info(logger, "Registering i/o interface %s", cfg_io->name);
    t_packet *packet = packet_new(NEW_INTERFACE);
    packet_addString(packet, cfg_io->name);
    packet_addString(packet, cfg_io->tipo_interfaz);
    int res = packet_send(packet, kernel_fd);
    packet_free(packet);

    return res;
}

void handleKernelIncomingMessage(uint8_t client_fd, uint8_t operation, t_buffer *buffer, void *_args)
{
    struct kernel_incoming_message_args *args = (struct kernel_incoming_message_args *)_args;
    int kernel_fd = args->kernel_fd;
    int memory_fd = args->memory_fd;
    t_log *logger = args->logger;
    t_io_config *config = args->config;
    // not freeing args coz they point to the same address in all requests

    uint32_t pid = packet_getUInt32(buffer);
    log_info(logger, "PID: %d - Operacion: %s", pid, io_op_to_string(operation));

    void sendDone(void)
    {
        interface_send_io_done(kernel_fd, interface_name, pid);
    }

    void doWork(int work)
    {
        msleep(config->unidad_trabajo * work);
    }

    switch (operation)
    {
    case IO_GEN_SLEEP:
    {
        t_interface_io_gen_sleep_msg *msg = malloc(sizeof(t_interface_io_gen_sleep_msg));
        interface_decode_io_gen_sleep(buffer, msg);

        doWork(msg->work_units);
        sendDone();

        interface_destroy_io_gen_sleep(msg);
        break;
    }
    case IO_STDIN_READ:
    {
        t_interface_io_stdin_read_msg *msg = malloc(sizeof(t_interface_io_stdin_read_msg));
        interface_decode_io_stdin_read(buffer, msg);

        // ask for prompt
        char *str = malloc(msg->size);
        prompt(msg->size, str);
        memory_send_write(memory_fd, pid, msg->address, msg->offset, str, msg->size);

        t_packet *res = packet_new(-1);
        if (packet_recv(memory_fd, res) == -1)
        {
            interface_destroy_io_stdin_read(msg);
            free(str);
            break;
        };
        if (res->op_code != WRITE_MEM_OK)
            log_info(logger, "ERROR WHILE SAVING TO MEMORY");

        sendDone();

        interface_destroy_io_stdin_read(msg);
        free(str);

        break;
    }
    case IO_STDOUT_WRITE:
    {
        do_work(1);

        t_interface_io_stdout_write_msg *msg = malloc(sizeof(t_interface_io_stdout_write_msg));
        interface_decode_io_stdout_write(buffer, msg);

        memory_send_read(memory_fd, pid, msg->address, msg->offset, msg->size);

        t_packet *res = packet_new(-1);
        if (packet_recv(memory_fd, res) == -1)
        {
            interface_destroy_io_stdout_write(msg);
            break;
        };

        if (res->op_code == READ_MEM_OK)
        {
            t_memory_read_ok_msg *ok_msg = malloc(sizeof(t_memory_read_ok_msg));
            memory_decode_read_ok(buffer, ok_msg, msg->size);
            log_info(logger, "MEMORY VALUE: %s", (char *)ok_msg->value);
        }

        sendDone();

        interface_destroy_io_stdout_write(msg);

        break;
    }
    case IO_FS_CREATE:
    {
        sendDone();
        break;
    }
    case IO_FS_DELETE:
    {
        sendDone();
        break;
    }
    case IO_FS_TRUNCATE:
    {
        sendDone();
        break;
    }
    case IO_FS_READ:
    {
        sendDone();
        break;
    }
    case IO_FS_WRITE:
    {
        sendDone();
        break;
    }
    case DESTROY_PROCESS:
    {
        char *response = packet_getString(buffer);
        log_info(logger, "Hi I am IO i received %s", response);
        free(response);
        break;
    }
    default:
    {
        log_error(logger, "undefined behaviour with opcode: %d", operation);
        break;
    }
    }
}

static char *io_op_to_string(t_opcode operation)
{
    switch (operation)
    {
    case IO_GEN_SLEEP:
        return "IO_GEN_SLEEP";
    case IO_STDIN_READ:
        return "IO_STDIN_READ";
    case IO_STDOUT_WRITE:
        return "IO_STDOUT_WRITE";
    case IO_FS_CREATE:
        return "IO_FS_CREATE";
    case IO_FS_DELETE:
        return "IO_FS_DELETE";
    case IO_FS_TRUNCATE:
        return "IO_FS_TRUNCATE";
    case IO_FS_READ:
        return "IO_FS_READ";
    case IO_FS_WRITE:
        return "IO_FS_WRITE";
    default:
        return "";
    }
}
