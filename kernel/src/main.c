#include <stdlib.h>
#include <stdio.h>
#include <sockets/sockets.h>
#include <string.h>
#include <errno.h>
#include <proto/proto.h>

int main(int argc, char *argv[])
{

    int interrupt_fd = socket_connectToServer(NULL, "5678");
    int dispatch_fd = socket_connectToServer(NULL, "8765");
    if (interrupt_fd == -1)
    {
        printf("err: %s", strerror(errno));
        return 1;
    }
    printf("connected to server\n");

    t_packet *packet = packet_new(EXEC_PROCESS);
    packet_addString(packet, "hello interrupt");
    packet_addUInt32(packet, 100);
    packet_addString(packet, "chau cpu");
    packet_send(packet, interrupt_fd);
    printf("packet sent to interrupt\n");
    packet_send(packet, dispatch_fd);
    printf("packet sent to dispatch\n");
    packet_free(packet);

    return 0;
}
