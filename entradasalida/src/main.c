#include <main.h>

static t_log *logger;
static t_IO_config *cfg_IO;

void config_init()
{
    t_config *config;
    config = config_create(CONFIG_PATH);
    if (!config)
    {
        perror("error al cargar el config");
        exit(1);
    }
    cfg_IO = malloc(sizeof(t_IO_config));

    cfg_IO->tipo_interfaz = config_get_string_value(config, "TIPO_INTERFAZ");
    cfg_IO->unidad_trabajo = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
    cfg_IO->ip_kernel = config_get_string_value(config, "IP_KERNEL");
    cfg_IO->puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");
    cfg_IO->ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    cfg_IO->puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    cfg_IO->path_base_dialfs = config_get_string_value(config, "PATH_BASE_DIALFS");
    cfg_IO->block_size = config_get_int_value(config, "BLOCK_SIZE");
    cfg_IO->block_count = config_get_int_value(config, "BLOCK_COUNT");

    config_destroy(config);
}

void IO_init()
{
    config_init();
    logger = log_create(LOG_PATH, PROCESS_NAME, 1, LOG_LEVEL);

    if (!logger)
    {
        perror("error al crear el logger");
        exit(1);
    }
}

void IO_close()
{
    log_destroy(logger);
    free(cfg_IO);
}

struct kernel_incoming_message_args
{
    int memory_fd;
    int kernel_fd;
};

void handleKernelIncomingMessage(uint8_t client_fd, uint8_t operation, t_buffer *buffer, void *_args)
{
    struct kernel_incoming_message_args *args = _args;

    switch (operation)
    {
    case IO_GEN_SLEEP:
        sleep(5);
        t_packet *packet = packet_new(IO_GEN_SLEEP);
        packet_send(packet, args->kernel_fd);
        break;
    }
}

int main(int argc, char *argv[])
{
    // basic setup
    IO_init();

    int memory_fd = socket_connectToServer(cfg_IO->ip_memoria, cfg_IO->puerto_memoria);
    int kernel_fd = socket_connectToServer(cfg_IO->ip_kernel, cfg_IO->puerto_kernel);
    if (memory_fd == -1 || kernel_fd == -1)
    {
        log_error(logger, "err: %s", strerror(errno));
        return 1;
    }
    log_info(logger, "connected to server\n");

    t_packet *packet = packet_new(SAVE_CONTEXT);
    printf("ingrese un numero: ");
    int a, b;
    scanf("%d", &a);
    printf("ingrese otro numero: ");
    scanf("%d", &b);
    packet_addUInt32(packet, a);
    packet_addUInt32(packet, b);
    packet_send(packet, memory_fd);
    printf("packet sent\n");
    packet_free(packet);

    packet = packet_new(IO_GEN_SLEEP);
    printf("ingrese un numero de segundos para dormir: ");
    scanf("%d", &a);
    packet_addUInt32(packet, a);
    packet_send(packet, kernel_fd);
    printf("packet sent\n");

    int status = 0;
    while (status != -1)
    {
        status = socket_read(kernel_fd, handleKernelIncomingMessage, NULL);
    }

    log_error(logger, "lost connection with the kernel");

    IO_close();

    return 0;
}
