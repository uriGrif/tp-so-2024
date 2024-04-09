#include <stdlib.h>
#include <stdio.h>
#include <sockets/sockets.h>
#include <string.h>
#include <errno.h>
#include <proto/proto.h>
#include<commons/log.h>
#include<commons/config.h>
#include<sys/signal.h>
#include<communications.h>

#define CONFIG_PATH  "kernel.config"
#define LOG_LEVEL  LOG_LEVEL_INFO
#define LOG_PATH "kernel.log"
#define PROCESS_NAME "Kernel"

static t_log* logger;
static t_config* config;
static int server_fd;

void init_kernell(){
    logger = log_create(LOG_PATH,PROCESS_NAME,1,LOG_LEVEL);
    if(!logger){
        perror("error al crear el logger");
        exit(1);
    }

    config = config_create(CONFIG_PATH);
    if(!config){
        perror("error al crear el config");
        exit(1);
    }

    char * port = config_get_string_value(config,"PUERTO_ESCUCHA");


    server_fd = socket_createTcpServer(NULL, port);
    if (server_fd == -1)
    {
        log_error(logger,"error: %s", strerror(errno));
        exit(1);
    }

    log_info(logger,"server starting");

}

void kernel_close(){
    log_destroy(logger);
    config_destroy(config);
}

void sighandler(int signal){
    kernel_close();
    close(server_fd);
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT,sighandler);

    int fd_cpu = socket_connectToServer(NULL, "5678");
    if (fd_cpu == -1)
    {
        printf("err: %s", strerror(errno));
        return 1;
    }
    printf("connected to server\n");

    t_packet *packet = packet_new(EXEC_PROCESS);
    packet_addString(packet, "hello from cpu");
    packet_addUInt32(packet, 100);
    packet_addString(packet,"chau cpu");
    packet_send(packet, fd_cpu);
    printf("packet sent\n");
    packet_free(packet);
    close(fd_cpu);


    init_kernell();
    server_listen(server_fd,logger,process_conn);
    kernel_close();

    // int client_fd = socket_acceptConns(server_fd);
    // if (client_fd == -1)
    // {
    //     printf("error: %s", strerror(errno));
    //     return 1;
    // }
    // t_packet *packet = packet_new(0);
    // int reading = packet_recv(client_fd, packet);
    // if (reading == -1)
    // {
    //     printf("error: %s", strerror(errno));
    //     return 1;
    // }

    // switch (packet->op_code)
    // {
    // case EXEC_PROCESS:
    //     printf("EXEC PROCESS\n");
    //     char *str = packet_getString(packet->buffer);
    //     uint32_t value = packet_getUInt32(packet->buffer);
    //     char *str2 = packet_getString(packet->buffer);
    //     printf("string: %s\n", str);
    //     printf("number: %d\n", value);
    //     printf("string2: %s\n", str2);
    //     free(str);
    //     free(str2);
    //     break;
    // default:
    //     printf("code not found");
    //     break;
    // }
    // packet_free(packet);
    return 0;
}
