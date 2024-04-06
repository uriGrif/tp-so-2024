#include <stdlib.h>
#include <stdio.h>
#include <sockets/sockets.h>
#include <string.h>
#include <errno.h>
#include <proto/proto.h>

int main(int argc, char *argv[])
{
    int server_fd = socket_createTcpServer(NULL, "8888");
    if (server_fd == -1)
    {
        printf("error: %s", strerror(errno));
        return 1;
    }

    printf("Server starter listening");

    int client_fd = socket_acceptConns(server_fd);
    if (client_fd == -1)
    {
        printf("error: %s", strerror(errno));
        return 1;
    }
    t_packet *packet = packet_new(0);
    int reading = packet_recv(client_fd, packet);
    if (reading == -1)
    {
        printf("error: %s", strerror(errno));
        return 1;
    }

    switch (packet->op_code)
    {
    case EXEC_PROCESS:
        printf("EXEC PROCESS\n");
        char *str = packet_getString(packet->buffer);
        uint32_t value = packet_getUInt32(packet->buffer);
        char *str2 = packet_getString(packet->buffer);
        printf("string: %s\n", str);
        printf("number: %d\n", value);
        printf("string2: %s\n", str2);
        free(str);
        free(str2);
        break;
    default:
        printf("code not found");
        break;
    }
    packet_free(packet);
    return 0;
}
