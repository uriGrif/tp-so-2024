#ifndef MMU_H
#define MMU

#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    uint32_t address;
    int offset;
} t_physical_address;

t_physical_address *translate_address(uint32_t virtual_address);

#endif