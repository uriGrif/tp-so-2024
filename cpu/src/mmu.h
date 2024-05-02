#ifndef MMU_H
#define MMU
typedef struct
{
    char *address;
    int offset;
} t_physical_address;

t_physical_address *translate_address(char *virtual_address);

#endif