#ifndef MMU_H
#define MMU

#include <stdlib.h>
#include <math.h>
#include <stdint.h>

typedef struct
{
    uint32_t page_number;
    uint32_t offset;
} t_physical_address;

t_physical_address *translate_address_4_bytes(uint32_t virtual_address, uint32_t page_size);

t_physical_address *translate_address_1_byte(uint8_t virtual_address, uint32_t page_size);

#endif