#ifndef KERNEL_H
#define KERNEL_H

#include <stdlib.h>
#include <stdio.h>
#include <sockets/sockets.h>
#include <string.h>
#include <errno.h>
#include <proto/proto.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <unistd.h>
#include <sys/signal.h>
#include <pthread.h>
#include <string.h>
#include <comms_kernel.h>
#include<config.h>
#include<pcb.h>
#include <console.h>

#define CONFIG_PATH "kernel.config"
#define LOG_LEVEL LOG_LEVEL_INFO
#define LOG_PATH "kernel.log"
#define PROCESS_NAME "Kernel"



#endif