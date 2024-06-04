#include <tlb.h>

// podria usarse un strategy en el tlb_insert parecido a como tenemos el scheduler 
// ojo que si la cantidad de entradas de la tlb es 0, siempre tiene que devolver -1 el tlb search
// quizas tambien se puede usar un strategy en tlb search, una funcion que solo devuelva -1
// idealmente no exportar la lista de la tlb, solo las funciones
// cuando se reemplaze un registro, liberarlo con un list_remove_and_destroy_by_condition
// no hace falta destruir toda la tlb al final creo



int tlb_search(uint32_t pid, uint32_t page) {
    // agregar el logger como parametros
    // loggear tlb hit o miss
    return -1;
}

void tlb_insert(uint32_t pid, uint32_t page, uint32_t frame) {}