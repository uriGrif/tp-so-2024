#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include<sockets/sockets.h>
#include<proto/proto.h>
#include<errno.h>

int main(int argc, char* argv[]) {
    //decir_hola("una Interfaz de Entrada/Salida");
     int fd = socket_connectToServer(NULL, "9999");
    if (fd == -1)
    {
        printf("err: %s", strerror(errno));
        return 1;
    }
    printf("connected to server\n");

    t_packet *packet = packet_new(SAVE_CONTEXT);
    printf("ingrese un numero\n");
    int a,b;
    scanf("%d",&a);
    printf("ingrese otro numero\n");
    scanf("%d",&b);
    packet_addUInt32(packet, a);
    packet_addUInt32(packet,b);
    packet_send(packet, fd);
    printf("packet sent\n");
    packet_free(packet);
    return 0;
}
