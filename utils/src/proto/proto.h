#ifndef PROTO_H
#define PROTO_H

#include <stdlib.h>
#include <stdint.h>
#include <sockets/sockets.h>

typedef struct
{
    uint32_t size;
    uint32_t offset;
    void *stream;
} t_buffer;

typedef enum
{
    // OPERACIONES KERNEL-CPU
    EXEC_PROCESS,
    INTERRUPT_EXEC,
    SAVE_CONTEXT,
    RESTORE_CONTEXT,
    // OPERACIONES CPU-MEMORIA
    READ_MEM,
    WRITE_MEM,
    RESIZE_PROCESS,
    // OPERACIONES KERNEL-MEMORIA
    CREATE_PROCESS,
    DESTROY_PROCESS,
    PAGE_FRAME,
    // OPERACIONES INTERFACES-KERNEL
    IO_GEN_SLEEP,
    IO_STD_IN_READ,
    IO_STD_OUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ
    // OPERACIONES INTERFACES-MEMORIA
} t_opcode;

typedef struct
{
    uint8_t op_code;
    t_buffer *buffer;

} t_packet;

t_packet *packet_new(uint8_t op_code);
void packet_free(t_packet *packet);
int packet_send(t_packet *packet, int client_fd);

int packet_recv(int fd, t_packet *packet);

uint32_t packet_getUInt32(t_buffer *buffer);
char *packet_getString(t_buffer *buffer);

int packet_addString(t_packet *packet, char *str);
int packet_addUInt32(t_packet *packet, uint32_t value);


#endif