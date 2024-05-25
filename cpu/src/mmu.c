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
            return;
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