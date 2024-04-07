#include <stdlib.h>
#include <stdio.h>
#include <sockets/sockets.h>
#include <proto/proto.h>
#include <errno.h>
#include <commons/log.h>
#include <commons/config.h>

#define CONFIG_PATH "cpu.config"
#define LOG_LEVEL LOG_LEVEL_INFO
#define LOG_PATH "cpu.log"
#define PROCESS_NAME "CPU"
#define MAX_CLIENTS 5

// creates the logger and loads the config
void cpu_setup(t_config **config, t_log **logger)
{
    *config = config_create(CONFIG_PATH);
    if (*config == NULL)
    {
        perror("error al cargar el config");
        exit(1);
    }

    *logger = log_create(LOG_PATH, PROCESS_NAME, 1, LOG_LEVEL);
    if (*logger == NULL)
    {
        perror("error al crear el logger");
        exit(1);
    }
}

void cpu_close(t_log *logger, t_config *config)
{
    log_destroy(logger);
    config_destroy(config);
}

/**
 * handlers
 */
struct handlers_args
{
    t_log *logger;
    t_config *config;
};

void request_handler(uint8_t client_fd, uint8_t op_code, t_buffer *buffer, void *_args)
{
    struct handlers_args *args = _args;
    t_log *logger = args->logger;

    switch (op_code)
    {
    case EXEC_PROCESS:
        log_info(logger, "EXEC PROCESS");
        char *str = packet_getString(buffer);
        uint32_t value = packet_getUInt32(buffer);
        char *str2 = packet_getString(buffer);
        log_info(logger, "string: %s", str);
        log_info(logger, "number: %d", value);
        log_info(logger, "string2: %s", str2);
        free(str);
        free(str2);
        break;
    default:
        log_error(logger, "undefined behaviour cop: %d", op_code);
        break;
    }
}

void on_new_connection(int client_fd, void *_args)
{
    struct handlers_args *args = _args;
    t_log *logger = args->logger;
    log_info(logger, "new client connected");
}

void on_connection_closed(int client_fd, void *_args)
{
    struct handlers_args *args = _args;
    t_log *logger = args->logger;
    log_error(logger, "client disconnect");
}

int main()
{
    t_log *logger = malloc(sizeof(t_log));
    t_config *config = malloc(sizeof(t_config));
    cpu_setup(&config, &logger);

    log_info(logger, "config loaded");
    char *server_ip = config_get_string_value(config, "IP_CPU");
    char *dispatch_port = config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
    char *interrupt_port = config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");

    int dispatch_fd = socket_createTcpServer(server_ip, dispatch_port);
    int interrupt_fd = socket_createTcpServer(server_ip, interrupt_port);

    if (dispatch_fd == -1 || interrupt_fd == -1)
    {
        log_error(logger, "socket_createTcpServer: [%s]", strerror(errno));
        return -1;
    }

    log_info(logger, "server started listening");

    struct handlers_args args;
    args.config = config;
    args.logger = logger;

    struct socket_AsyncServerConf conf;
    conf.fds_size = 2;
    conf.server_fds = malloc(conf.fds_size * sizeof(int *));
    conf.server_fds[0] = &dispatch_fd;
    conf.server_fds[1] = &interrupt_fd;
    conf.handlers_args = &args;
    conf.max_clients = MAX_CLIENTS;
    conf.requestHandler = request_handler;
    conf.onConnectionClosed = on_connection_closed;
    conf.onNewConnection = on_new_connection;

    int status = socket_acceptConnsAsync(&conf);

    if (status == -1)
    {
        printf("the server has quit unexpectedly: %s\n", strerror(errno));
        return 1;
    }

    // should never reach this point
    return 0;
}