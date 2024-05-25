#ifndef TLB_H
#define TLB_H

#include <commons/collections/list.h>
#include <stdint.h>
#include <time.h>

typedef struct {
    t_list entries;
    uint32_t entries_amount;
} t_tlb;

typedef struct {
    uint32_t pid;
    uint32_t page;
    uint32_t frame;
    time_t last_use;
} t_tlb_row;

extern t_tlb TLB;

int tlb_search(uint32_t pid, uint32_t page); // returns -1 if not found

void tlb_insert(uint32_t pid, uint32_t page, uint32_t frame);

#endif