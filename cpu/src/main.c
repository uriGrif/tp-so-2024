#include <stdlib.h>
#include <stdio.h>
#include <sockets/sockets.h>
#include <proto/proto.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    int fd = socket_connectToServer(NULL, "8888");
    // int fd = crear_conexionNULL, "8888");
    if (fd == -1)
    {
        printf("err: %s", strerror(errno));
        return 1;
    }
    printf("connected to server\n");

    t_packet *packet = packet_new(EXEC_PROCESS);
    packet_addString(packet, "hello from cpu");
    packet_addUInt32(packet, 100);
    packet_send(packet, fd);
    printf("packet sent\n");
    packet_free(packet);

    return 0;
}
