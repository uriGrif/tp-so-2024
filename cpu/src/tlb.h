#ifndef TLB_H
#define TLB_H

#include <commons/collections/list.h>
#include <commons/log.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

typedef enum {
    FIFO_TLB,
    LRU_TLB
} t_tlb_algorithm;

typedef struct {
    t_list *entries;
    uint32_t entries_amount;
    t_tlb_algorithm algorithm;
} t_tlb;

typedef struct {
    uint32_t pid;
    uint32_t page;
    uint32_t frame;
    time_t last_use;
} t_tlb_row;

extern t_tlb TLB;

int tlb_search(uint32_t pid, uint32_t page, t_log *logger); // returns -1 if not found

void tlb_insert(uint32_t pid, uint32_t page, uint32_t frame);

#endif