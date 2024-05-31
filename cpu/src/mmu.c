#include <mmu.h>

uint32_t get_frame(int fd_mem, uint32_t pid, uint32_t page_number, t_log *logger) {
    uint32_t frame_number = tlb_search(pid, page_number);
    if (frame_number == -1) {
        memory_send_page_table_access(fd_mem, pid, page_number);
        t_packet *packet = packet_new(-1);
        if (packet_recv(fd_mem, packet) == -1)
        {
            log_error(logger, "error al leer de la memoria");
            packet_free(packet);
            return 0;
        }
        frame_number = memory_decode_frame_number(packet->buffer);
        packet_free(packet);
    }
    return frame_number;
}

uint32_t translate_address_4_bytes(int fd_mem, uint32_t pid, uint32_t virtual_address, uint32_t page_size, t_log *logger)
{
    uint32_t page_number = floor(virtual_address / page_size);
    uint32_t offset = virtual_address - (page_number * page_size);

    uint32_t frame_number = get_frame(fd_mem, pid, page_number, logger);

    uint32_t physical_address = frame_number * page_size + offset;
    return physical_address;
}

uint32_t translate_address_1_byte(int fd_mem, uint32_t pid, uint8_t virtual_address, uint32_t page_size, t_log *logger)
{
    uint32_t page_number = floor(virtual_address / page_size);
    uint32_t offset = virtual_address - (page_number * page_size);

    uint32_t frame_number = get_frame(fd_mem, pid, page_number, logger);

    uint32_t physical_address = frame_number * page_size + offset;
    return physical_address;
}

t_access_to_memory* access_to_memory_create(uint32_t logical_address, uint32_t total_bytes, uint32_t page_size) {
    t_access_to_memory *result = malloc(sizeof(t_access_to_memory));

    uint32_t first_page = floor(logical_address / page_size);

    uint32_t extra_page = logical_address % page_size == 0 ? 0 : 1;
    result->pages_amount = total_bytes / page_size + extra_page;
    result->addresses = malloc(sizeof(uint32_t) * result->pages_amount);
    result->bytes_to_access = malloc(sizeof(uint32_t) * result->pages_amount);

    int i = 0;

    while (total_bytes > 0)
    {
        result->addresses[i] = first_page + i;
        uint32_t offset = logical_address - (first_page + i) * page_size;
        if (total_bytes >= page_size)
        {
            result->bytes_to_access[i] = page_size - offset;
        }
        else
        {
            result->bytes_to_access[i] = total_bytes;
        }
        total_bytes -= result->bytes_to_access[i];
        logical_address += result->bytes_to_access[i];
        i++;
    }
    
    return result;
}

void access_to_memory_free(t_access_to_memory *a) {
    free(a->addresses);
    free(a->bytes_to_access);
    free(a);
}
