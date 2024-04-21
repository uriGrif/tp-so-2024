#ifndef IO_KERNEL_CONNS_H
#define IO_KERNEL_CONNS_H

#include <stdlib.h>
#include <stdio.h>
#include <proto/proto.h>
#include <errno.h>
#include <commons/log.h>
#include <config.h>
#include <utils/utlis.h>

int registerResourceInKernel(int kernel_fd, t_log *logger, t_io_config *config);

struct kernel_incoming_message_args
{
    int kernel_fd;
    t_log *logger;
    t_io_config *config;
};

void handleKernelIncomingMessage(uint8_t client_fd, uint8_t operation, t_buffer *buffer, void *_args);

#endif