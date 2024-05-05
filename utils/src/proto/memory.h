#include <proto/proto.h>

typedef struct
{
    uint32_t pid;
    uint32_t page_number;
    uint32_t offset;
    uint32_t size;
} t_memory_read_msg;

typedef struct
{
    void *value;
} t_memory_read_ok_msg;

typedef struct
{
    int pid;
    uint32_t page_number;
    uint32_t offset;
    void *value;
    uint32_t size;
} t_memory_write_msg;

int memory_send_read(int fd, int pid, uint32_t page_number, uint32_t offset, uint32_t size);
void memory_decode_read(t_buffer *buffer, t_memory_read_msg *msg);
void memory_destroy_read(t_memory_read_msg *msg);

int memory_send_read_ok(int fd, void *value, uint32_t size);
void memory_decode_read_ok(t_buffer *buffer, t_memory_read_ok_msg *msg, uint32_t size);
void memory_destroy_read_ok(t_memory_read_ok_msg *msg);

int memory_send_write(int fd, int pid, uint32_t page_number, uint32_t offset, uint32_t size, void *value);
void memory_decode_write(t_buffer *buffer, t_memory_write_msg *msg);
void memory_destroy_write(t_memory_write_msg *msg);

int memory_send_write_ok(int fd);