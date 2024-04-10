#include <main.h>

t_log *logger;
static int dispatch_fd;
static int interrupt_fd;
static t_config *config;
static t_cpu_config* cfg_cpu;

void config_init()
{
    config = config_create(CONFIG_PATH);
    if (!config)
    {
        perror("error al cargar el config");
        exit(1);
    }

    cfg_cpu = malloc(sizeof(t_cpu_config));

    cfg_cpu->ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    cfg_cpu->puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    cfg_cpu->puerto_escucha_dispatch = config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
    cfg_cpu->puerto_escucha_interrupt = config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");
    cfg_cpu->cantidad_entradas_tlb = config_get_int_value(config, "CANTIDAD_ENTRADAS_TLB");
    cfg_cpu->algoritmo_tlb = config_get_string_value(config, "ALGORITMO_TLB");

    //printf("puerto me %s %s\n", cfg_CPU->puerto_escucha_interrupt, cfg_CPU->puerto_escucha_dispatch);

}


void cpu_init()
{
    config_init();
    logger = log_create(LOG_PATH, PROCESS_NAME, 1, LOG_LEVEL);
    if (!logger)
    {
        perror("error al crear el logger");
        exit(1);
    }

    dispatch_fd = socket_createTcpServer(NULL, cfg_cpu->puerto_escucha_dispatch);
    interrupt_fd = socket_createTcpServer(NULL, cfg_cpu->puerto_escucha_interrupt);

    if (dispatch_fd == -1 || interrupt_fd == -1)
    {
        printf("error: %s", strerror(errno));
        exit(1);
    }

    log_info(logger, "server starting");
}

void cpu_close()
{
    log_destroy(logger);
    free(cfg_cpu);
    config_destroy(config);
}

int main(int argc, char *argv[])
{
    cpu_init();

    int fd_memoria = socket_connectToServer(cfg_cpu->ip_memoria, cfg_cpu->puerto_memoria);
    t_packet *packet = packet_new(READ_MEM);
    packet_addString(packet, "Hello Memory! I'm the CPU!");
    packet_send(packet, fd_memoria);
    packet_free(packet);

    // POR AHORA NOMAS
    socket_freeConn(fd_memoria);

    fd_set readset, tempset;
    struct timeval tv;
    int cli_dispatch_fd;
    int cli_intr_fd;
    int maxfd;
    int result;

    FD_ZERO(&readset);
    FD_SET(dispatch_fd, &readset);
    FD_SET(interrupt_fd, &readset);
    maxfd = dispatch_fd > interrupt_fd ? dispatch_fd : interrupt_fd;

    do
    {
        memcpy(&tempset, &readset, sizeof(tempset));
        tv.tv_sec = 30;
        tv.tv_usec = 0;
        result = select(maxfd + 1, &tempset, NULL, NULL, &tv);

        if (0 == result)
            log_warning(logger, "Select timeout!");

        else if (result < 0 && errno != EINTR)
            log_error(logger, "Select error");
        else if (result > 0)
        {
            if (FD_ISSET(dispatch_fd, &tempset))
            {
                cli_dispatch_fd = socket_acceptConns(dispatch_fd);

                if (cli_dispatch_fd < 0)
                {
                    log_error(logger, "error en accept");
                }
                else
                {
                    FD_SET(cli_dispatch_fd, &tempset);
                    maxfd = (maxfd < cli_dispatch_fd) ? cli_dispatch_fd : maxfd;
                }
                FD_CLR(dispatch_fd, &tempset);
            }
            if (FD_ISSET(interrupt_fd, &tempset))
            {
                cli_intr_fd = socket_acceptConns(interrupt_fd);

                if (cli_intr_fd < 0)
                {
                    log_error(logger, "error en accept");
                }
                else
                {
                    FD_SET(cli_intr_fd, &tempset);
                    maxfd = (maxfd < cli_intr_fd) ? cli_intr_fd : maxfd;
                }
                FD_CLR(interrupt_fd, &tempset);
            }

            for (int i = 0; i < maxfd + 1; i++)
            {
                if (FD_ISSET(i, &tempset))
                {
                    t_packet *packet = packet_new(0);
                    if (packet_recv(i, packet) != -1)
                    {
                        switch (packet->op_code)
                        {
                        case EXEC_PROCESS:
                        {
                            log_info(logger, "EXEC PROCESS\n");
                            char *str = packet_getString(packet->buffer);
                            uint32_t value = packet_getUInt32(packet->buffer);
                            char *str2 = packet_getString(packet->buffer);
                            log_info(logger, "string: %s\n", str);
                            log_info(logger, "number: %d\n", value);
                            log_info(logger, "string2: %s\n", str2);
                            free(str);
                            free(str2);
                            break;
                        }
                        case INTERRUPT_EXEC:
                        {
                            log_info(logger, "INTERRUPT EXEC\n");
                            char *str = packet_getString(packet->buffer);
                            uint32_t value = packet_getUInt32(packet->buffer);
                            char *str2 = packet_getString(packet->buffer);
                            log_info(logger, "string: %s\n", str);
                            log_info(logger, "number: %d\n", value);
                            log_info(logger, "string2: %s\n", str2);
                            free(str);
                            free(str2);
                            break;
                        }
                        case -1:
                            log_error(logger, "client disconnect");
                            packet_free(packet);
                            continue;

                        default:
                            log_error(logger, "undefined behaviour cop: %d", packet->op_code);
                            packet_free(packet);
                            continue;
                            ;
                        }

                        packet_free(packet);
                    }
                    else
                    {
                        packet_free(packet);
                        close(i);
                        FD_CLR(i, &readset);
                    }
                }
            }
        }
    } while (1);

    cpu_close();

    return 0;
}
