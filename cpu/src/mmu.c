#include <mmu.h>

// this would map the virtual address to the corresponding physical address stored in memory
t_physical_address *translate_address_4_bytes(uint32_t virtual_address)
{
    t_physical_address *physical_address = malloc(sizeof(t_physical_address));
    // for now we just plug mock values
    physical_address->page_number = floor(virtual_address/PAGE_SIZE);
    physical_address->offset = virtual_address - (physical_address->page_number *PAGE_SIZE);
    return physical_address;
}


t_physical_address *translate_address_1_byte(uint8_t virtual_address)
{
    t_physical_address *physical_address = malloc(sizeof(t_physical_address));
    // for now we just plug mock values
    physical_address->page_number = floor(virtual_address/PAGE_SIZE);
    physical_address->offset = virtual_address - (physical_address->page_number *PAGE_SIZE);
    return physical_address;
}