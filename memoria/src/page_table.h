#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include <stdint.h>
#include <commons/collections/list.h>

typedef struct {
    uint32_t pid;
    t_list frames;
} t_page_table;

void create_page_table(uint32_t pid, t_list *page_table_list);
void delete_page_table(uint32_t pid, t_list *page_table_list);
void add_frame_to_page_table(uint32_t pid, t_list *page_table_list);
void delete_last_frame_of_page_table(uint32_t pid, t_list *page_table_list);
uint32_t get_frame_from_page_table(uint32_t pid, uint32_t page_number, t_list *page_table_list);

#endif