#include <main.h>

static t_log *logger;
static t_config *config;
static t_io_config *cfg_io;
static char* interface_name;
static char *config_path;
int memory_fd;
int kernel_fd;

void config_init(void)
{
    config = config_create(config_path);
    if (!config)
    {
        perror("error al cargar el config");
        exit(1);
    }
    cfg_io = malloc(sizeof(t_io_config));
    cfg_io->name = interface_name;
    cfg_io->tipo_interfaz = config_get_string_value(config, "TIPO_INTERFAZ");
    cfg_io->unidad_trabajo = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
    cfg_io->ip_kernel = config_get_string_value(config, "IP_KERNEL");
    cfg_io->puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");
    cfg_io->ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    cfg_io->puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    cfg_io->path_base_dialfs = config_get_string_value(config, "PATH_BASE_DIALFS");
    cfg_io->block_size = config_get_int_value(config, "BLOCK_SIZE");
    cfg_io->block_count = config_get_int_value(config, "BLOCK_COUNT");
}

void io_init(int argc, char **argv)
{
    logger = log_create(LOG_PATH, PROCESS_NAME, 1, LOG_LEVEL);
    if (!logger)
    {
        perror("error al crear el logger");
        exit(1);
    }

    // get path and name from arguments
    if (argc < 3)
    {
        log_error(logger, "You must provide the name and the path to the config file");
        exit(1);
    }
    interface_name = argv[1];
    config_path = argv[2];

    config_init();
}

void io_close(void)
{
    log_destroy(logger);
    free(cfg_io);
    config_destroy(config);
}

int main(int argc, char *argv[])
{
    // basic setup
    io_init(argc, argv);

    // connect to servers
    memory_fd = socket_connectToServer(cfg_io->ip_memoria, cfg_io->puerto_memoria);
    kernel_fd = socket_connectToServer(cfg_io->ip_kernel, cfg_io->puerto_kernel);
    if (memory_fd == -1 || kernel_fd == -1)
    {
        log_error(logger, "err: %s", strerror(errno));
        return 1;
    }
    log_info(logger, "connected to server\n");

    // send registration to kernel
    int res = registerResourceInKernel(kernel_fd, logger, cfg_io);
    if (res == -1)
    {
        log_error(logger, "could not register resource to the kernel, reason: %s\n", strerror(errno));
        exit(1);
    }

    int status = 0;

    struct kernel_incoming_message_args args;
    args.kernel_fd = kernel_fd;
    args.logger = logger;
    args.config = cfg_io;

    while (status != -1)
    {
        status = socket_read(kernel_fd, handleKernelIncomingMessage, (void *)&args);
    }

    log_error(logger, "lost connection with the kernel");

    io_close();

    return 0;
}
