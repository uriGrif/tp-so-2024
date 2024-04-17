#include <main.h>

t_log *logger;
static int dispatch_fd;
static int interrupt_fd;
static t_config *config;
static t_cpu_config *cfg_cpu;
static pthread_t thread_intr;
t_cpu_registers registers;

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

    log_info(logger, "server starting");
}

void start_interrupt_listener()
{
    // hago el accept y el manejo de las interrupts en otro hilo
    t_process_conn_args *interrupt_args = malloc(sizeof(t_process_conn_args));
    interrupt_args->logger = logger;
    interrupt_args->fd = interrupt_fd;
    
    // TODO: agregar la queue en los argumentos y un mutex


    pthread_create(&thread_intr, NULL, (void *)handle_interrupt, (void *)interrupt_args);
    pthread_detach(thread_intr);
}

void cpu_close()
{
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

void load_pcb(t_packet* pcb, int32_t* pid, t_registers* registers) {
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

    // espero a que el kernel se conecte a dispatc
    int cli_dispatch_fd;
    socket_acceptConns(dispatch_fd);

    // declaro variables importantes
    int32_t current_pid = -1;
    t_instruction current_instruction;
    char *instruction_text;
    t_queue* interruption_queue = queue_create();

    t_packet pcb_packet;

    // arranco el ciclo...
    while (1)
    {
        if (current_pid == -1) {
            packet_recv(cli_dispatch_fd, &pcb_packet); // es bloqueante
            load_pcb(&pcb_packet, &current_pid, &registers);
        }

        instruction_text = fetch(fd_memoria, registers.pc);

        current_instruction = decode(instruction_text, &registers);

        execute(&current_instruction);

        check_interrupt(&interruption_queue, cli_dispatch_fd);
    }
    
    // cpu_close();

    return 0;
}
