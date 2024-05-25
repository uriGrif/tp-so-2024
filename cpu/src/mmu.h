#ifndef MMU_H
#define MMU

#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <commons/string.h>
#include <tlb.h>
#include <sys/socket.h>
#include <proto/proto.h>
#include <commons/log.h>

uint32_t translate_address_4_bytes(int fd_mem, uint32_t pid, uint32_t virtual_address, uint32_t page_size, t_log *logger);

uint32_t translate_address_1_byte(int fd_mem, uint32_t pid, uint8_t virtual_address, uint32_t page_size, t_log *logger);

#endif