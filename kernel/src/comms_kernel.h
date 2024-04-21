#ifndef COMMUNICATIONS_H_KERNEL
#define COMMUNICATIONS_H_KERNEL

#include <proto/proto.h>
#include <sockets/sockets.h>
#include <interface.h>

struct req_io_gen_sleep
{
    char *interface_name;
    uint32_t work_units;
};

void handle_connections(void *void_args);

#endif