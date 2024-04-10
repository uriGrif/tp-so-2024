#include <main.h>

static t_log *logger;
static t_config *config;
static int fd_server;

void init_memory()
{
    config = config_create(CONFIG_PATH);
    if (!config)
    {
        perror("error al cargar el config");
        exit(1);
    }

    logger = log_create(LOG_PATH, PROCESS_NAME, 1, LOG_LEVEL);
    if (!logger)
    {
        perror("error al crear el logger");
        exit(1);
    }

    char *port = config_get_string_value(config, "PUERTO_ESCUCHA");

    fd_server = socket_createTcpServer(NULL, port);

    if (fd_server == -1)
    {
        log_error(logger, "error %s", strerror(errno));
        exit(1);
    }

    log_info(logger, "server starting");
}

void memory_close()
{
    log_destroy(logger);
    config_destroy(config);
}

void sighandler(int signal)
{
    memory_close();
    socket_freeConn(fd_server);
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
