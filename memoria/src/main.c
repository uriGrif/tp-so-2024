#include <main.h>

static t_log *logger;
static t_config *config;
t_mem_config *cfg_mem;
static int fd_server;

static void config_init(void)
{
    config = config_create(CONFIG_PATH);
    if (!config)
    {
        log_error(logger, "error al cargar el config");
        exit(1);
    }

    cfg_mem = malloc(sizeof(t_mem_config));
    cfg_mem->puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    cfg_mem->tam_memoria = config_get_int_value(config, "TAM_MEMORIA");
    cfg_mem->tam_pagina = config_get_int_value(config, "TAM_PAGINA");
    cfg_mem->path_instrucciones = config_get_string_value(config, "PATH_INSTRUCCIONES");
    cfg_mem->retardo_respuesta = config_get_int_value(config, "RETARDO_RESPUESTA");
}

static void init_memory(void)
{
    logger = log_create(LOG_PATH, PROCESS_NAME, 1, LOG_LEVEL);
    if (!logger)
    {
        perror("error al crear el logger");
        exit(1);
    }

    config_init();

    fd_server = socket_createTcpServer(NULL, cfg_mem->puerto_escucha);

    if (fd_server == -1)
    {
        log_error(logger, "error %s", strerror(errno));
        exit(1);
    }

    const int enable = 1;
    if (setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        log_error(logger, "setsockopt(SO_REUSEADDR) failed");

    init_process_list();

    log_info(logger, "server starting");
}

static void memory_close(void)
{
    log_destroy(logger);
    free(cfg_mem);
    config_destroy(config);
    process_list_destroy();
}

void sighandler(int signal)
{
    memory_close();
    close(fd_server);
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sighandler);
    init_memory();
    socket_acceptOnDemand(fd_server, logger, process_conn);
    memory_close();
    return 0;
}
