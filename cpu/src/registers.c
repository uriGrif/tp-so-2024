#include<registers.h>

const t_register REGISTERS[] = {"PC",&registers.pc,sizeof(uint32_t),
    {"AX",&registers.ax,sizeof(uint8_t)},
    {"BX",&registers.bx,sizeof(uint8_t)},
    {"CX",&registers.cx,sizeof(uint8_t)},
    {"DX",&registers.dx,sizeof(uint8_t)},
    {"EAX",&registers.eax,sizeof(uint32_t)},
    {"EBX",&registers.ebx,sizeof(uint32_t)},
    {"ECX",&registers.ecx,sizeof(uint32_t)},
    {"EDX",&registers.edx,sizeof(uint32_t)},
    {"SI",&registers.si,sizeof(uint32_t)},
    {"DI",&registers.di,sizeof(uint32_t)},
    {NULL,NULL,0}};

