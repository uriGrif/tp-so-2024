#include <mmu.h>

// this would map the virtual address to the corresponding physical address stored in memory
t_physical_address *translate_address(char *virtual_address)
{
    t_physical_address *physical_address = malloc(sizeof(t_physical_address));
    // for now we just plug mock values
    physical_address->address = virtual_address;
    physical_address->offset = 0;
    return virtual_address;
};