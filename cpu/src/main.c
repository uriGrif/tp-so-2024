#include <main.h>

t_log *logger;
static int dispatch_fd;
static int interrupt_fd;
int cli_dispatch_fd = -1;
static t_config *config;
static t_cpu_config *cfg_cpu;
static pthread_t thread_intr;
t_exec_context context;

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

    // printf("puerto me %s %s\n", cfg_CPU->puerto_escucha_interrupt, cfg_CPU->puerto_escucha_dispatch);
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

    const int enable = 1;
    if (setsockopt(dispatch_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");
    if (setsockopt(interrupt_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    if (dispatch_fd == -1 || interrupt_fd == -1)
    {
        printf("error: %s", strerror(errno));
        exit(1);
    }
    init_sem_interrupt();
    log_info(logger, "server starting");
}

void start_interrupt_listener()
{
    // hago el accept y el manejo de las interrupts en otro hilo

    t_process_conn_args *interrupt_args = malloc(sizeof(t_process_conn_args));
    interrupt_args->fd = interrupt_fd;
    interrupt_args->logger = logger;

    // TODO: agregar la queue en los argumentos y un mutex

    pthread_create(&thread_intr, NULL, (void *)handle_interrupt, (void *)interrupt_args);
    pthread_detach(thread_intr);
}

void cpu_close()
{
    close_sem_interrupt();
    pthread_cancel(thread_intr);
    log_destroy(logger);
    free(cfg_cpu);
    config_destroy(config);
}

void sighandler(int signal)
{
    cpu_close();
    close(dispatch_fd);
    close(interrupt_fd);
    exit(0);
}

void load_pcb(t_packet *pcb)
{
    // TODO
    // leo el packet y cargo el contexto que me llego
}

int main(int argc, char *argv[])
{

    signal(SIGINT, sighandler);
    // dejo en listen los fds dispatch e interrupt
    cpu_init();
    start_interrupt_listener();

    // me conecto a memoria
    int fd_memoria = socket_connectToServer(cfg_cpu->ip_memoria, cfg_cpu->puerto_memoria);
    t_packet *packet = packet_new(READ_MEM);
    packet_addString(packet, "Hello Memory! I'm the CPU!");
    packet_send(packet, fd_memoria);
    packet_free(packet);
    // // PRUEBA MEMORIA
    // context.pid = 3;
    // // for(; context.registers.pc<3; context.registers.pc++){
    // // char* next_instruction = fetch(fd_memoria,logger);
    // // decode_and_execute(next_instruction,logger);
    // // }

    // // log_debug(logger,"AX: %d BX: %d", context.registers.ax, context.registers.bx);
    // // log_debug(logger,"EL BX del context: %d", context.registers.bx);

    // espero a que el kernel se conecte a dispatch

    // declaro variables importantes

    // intento recibir la conexion del kernel
    while (cli_dispatch_fd == -1)
    {
        cli_dispatch_fd = socket_acceptConns(dispatch_fd);

        // arranco el ciclo...
        while (1)
        {
            if (context.pid == 0)
            {
                t_packet *packet = packet_new(-1);
                if (packet_recv(cli_dispatch_fd, packet) == -1)
                { // es bloqueante
                    log_error(logger, "se desconecto el kernel de dispatch");
                    packet_free(packet);
                    cli_dispatch_fd = -1;
                    break;
                }
                packet_get_context(packet->buffer, &context);
                packet_free(packet);
                log_debug(logger, "me llego: pid: %d, quantum: %d", context.pid, context.quantum);
            }
            char *next_instruction = fetch(fd_memoria, logger);
            context.registers.pc++;
            decode_and_execute(next_instruction, logger);
            check_interrupt();

            if (context.registers.pc == 7)
            {
                log_debug(logger, "AX: %d", context.registers.ax);
                context.pid = 0;
            }
        }
    }

    // cpu_close();

    return 0;
}
