#include <tlb.h>

// podria usarse un strategy en el tlb_insert parecido a como tenemos el scheduler 
// ojo que si la cantidad de entradas de la tlb es 0, siempre tiene que devolver -1 el tlb search
// quizas tambien se puede usar un strategy en tlb search, una funcion que solo devuelva -1
// idealmente no exportar la lista de la tlb, solo las funciones
// cuando se reemplaze un registro, liberarlo con un list_remove_and_destroy_by_condition
// no hace falta destruir toda la tlb al final creo

int tlb_search(uint32_t pid, uint32_t page, t_log *logger) {
    
    bool find_entry(void *elem) {
        t_tlb_row *temp = (t_tlb_row *) elem;
        return pid == temp->pid && page == temp->page;
    }
    
    t_tlb_row *entry = (t_tlb_row *) list_find(TLB.entries, find_entry);

    if (!entry) {
        log_info(logger, "PID: %d - TLB MISS - Pagina: %d", pid, page);
        return -1;
    }

    log_info(logger, "PID: %d - TLB HIT - Pagina: %d", pid, page);

    if (TLB.algorithm == LRU_TLB) {
        time(&entry->last_use);
    }
    
    return entry->frame;
}

void tlb_insert(uint32_t pid, uint32_t page, uint32_t frame) {
    int current_size = list_size(TLB.entries);
    if(current_size < TLB.entries_amount){
        t_tlb_row *entry = malloc(sizeof(t_tlb_row));
        entry->pid = pid;
        entry->page = page;
        entry->frame = frame;
        time(&entry->last_use);
        list_add(TLB.entries, entry);
        return;
    }
    
    void *min_tlb(void *elem1, void *elem2) {
        t_tlb_row *entry1 = (t_tlb_row *) elem1;
        t_tlb_row *entry2 = (t_tlb_row *) elem2;
        if(difftime(entry1->last_use, entry2->last_use) < 0)
            return entry1;
        else
            return entry2;
    }
    
    t_tlb_row *lowest_timestamp = (t_tlb_row*) list_get_minimum(TLB.entries, min_tlb);

    printf("\nSaque entrada pagina: %d del pid: %d y puse la pagina: %d del pid: %d\n", lowest_timestamp->page, lowest_timestamp->pid, page, pid);

    lowest_timestamp->pid = pid;
    lowest_timestamp->page = page;
    lowest_timestamp->frame = frame;
    time(&lowest_timestamp->last_use);
}