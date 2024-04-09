#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include<sockets/sockets.h>
#include<proto/proto.h>
#include<errno.h>

int main(int argc, char* argv[]) {
    //decir_hola("una Interfaz de Entrada/Salida");
     int fd = socket_connectToServer(NULL, "9999");
     int fd_kernel = socket_connectToServer(NULL,"8888");
    if (fd == -1 || fd_kernel == -1)
    {
        printf("err: %s", strerror(errno));
        return 1;
    }
    printf("connected to server\n");

    t_packet *packet = packet_new(SAVE_CONTEXT);
    printf("ingrese un numero: ");
    int a,b;
    scanf("%d",&a);
    printf("ingrese otro numero: ");
    scanf("%d",&b);
    packet_addUInt32(packet, a);
    packet_addUInt32(packet,b);
    packet_send(packet, fd);
    printf("packet sent\n");
    packet_free(packet);
    close(fd);

    packet = packet_new(IO_GEN_SLEEP);
    printf("ingrese un numero de segundos para dormir: ");
    scanf("%d",&a);
    packet_addUInt32(packet,a);
    packet_send(packet,fd_kernel);
    printf("packet sent\n");
    packet_free(packet);
    close(fd_kernel);
    return 0;;
}
