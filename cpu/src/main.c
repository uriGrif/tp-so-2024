#include <main.h>

t_log *logger;
static int dispatch_fd;
static int interrupt_fd;
int cli_dispatch_fd = -1;
static t_config *config;
static t_cpu_config *cfg_cpu;
static pthread_t thread_intr;
t_exec_context context;

static void config_init()
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

static void cpu_init()
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
    log_info(logger, "server starting");
}

static void start_interrupt_listener()
{
    interrupt_mutex_init();
    // hago el accept y el manejo de las interrupts en otro hilo

    t_process_conn_args *interrupt_args = malloc(sizeof(t_process_conn_args));
    interrupt_args->fd = interrupt_fd;
    interrupt_args->logger = logger;

    // TODO: agregar la queue en los argumentos y un mutex

    pthread_create(&thread_intr, NULL, (void *)handle_interrupt, (void *)interrupt_args);
    pthread_detach(thread_intr);
}

static void cpu_close()
{
    interrupt_mutex_destroy();
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

int main(int argc, char *argv[])
{

    signal(SIGINT, sighandler);
    // dejo en listen los fds dispatch e interrupt
    cpu_init();
    start_interrupt_listener();

    // me conecto a memoria
    fd_memory = socket_connectToServer(cfg_cpu->ip_memoria, cfg_cpu->puerto_memoria);

    send_mem_handshake(logger);
    // intento recibir la conexion del kernel
    while (cli_dispatch_fd == -1)
    {
        cli_dispatch_fd = socket_acceptConns(dispatch_fd);

        // arranco el ciclo...
        while (1)
        {
            current_exec_process_has_finished = 0;
            if (wait_for_context(&context) == -1)
            {
                log_error(logger, "se desconecto el kernel de dispatch");
                cli_dispatch_fd = -1;
                break;
            }
            log_debug(logger, "me llego: pid: %d, quantum: %d, AX: %u", context.pid, context.quantum, context.registers.ax);

            while (!current_exec_process_has_finished)
            {
                char *next_instruction = fetch(fd_memory, logger);
                if(!next_instruction)
                    break;
                context.registers.pc++;
                decode_and_execute(next_instruction, logger);
                check_interrupt(logger);
            }
        }
    }

    // cpu_close();

    return 0;
}
