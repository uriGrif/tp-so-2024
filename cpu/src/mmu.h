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
#include <proto/memory.h>

typedef struct {
    uint32_t pages_amount; // cantidad de paginas a las que sera necesario acceder
    uint32_t *addresses; // array del tamanio de la cantidad de paginas (cada elemento es una direccion en cada pagina) (son direcciones logicas, vamos agarrando una por una y las traducimos a fisicas)
    uint32_t *bytes_to_access; // array (cada elemento son la cantidad de bytes a acceder desde cada direccion
} t_access_to_memory;

uint32_t translate_address_4_bytes(int fd_mem, uint32_t pid, uint32_t virtual_address, uint32_t page_size, t_log *logger);

uint32_t translate_address_1_byte(int fd_mem, uint32_t pid, uint8_t virtual_address, uint32_t page_size, t_log *logger);

t_access_to_memory* access_to_memory_create(uint32_t logical_address, uint32_t total_bytes, uint32_t page_size);

void access_to_memory_free(t_access_to_memory *a);

#endif