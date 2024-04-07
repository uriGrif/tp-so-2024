#include <stdlib.h>
#include <stdio.h>
#include <sockets/sockets.h>
#include <string.h>
#include <errno.h>
#include <proto/proto.h>

int main(int argc, char *argv[])
{

    int fd = socket_connectToServer(NULL, "5678");
    if (fd == -1)
    {
        printf("err: %s", strerror(errno));
        return 1;
    }
    printf("connected to server\n");

    t_packet *packet = packet_new(EXEC_PROCESS);
    packet_addString(packet, "hello from cpu");
    packet_addUInt32(packet, 100);
    packet_addString(packet, "chau cpu");
    packet_send(packet, fd);
    printf("packet sent\n");
    packet_free(packet);

    return 0;
}
