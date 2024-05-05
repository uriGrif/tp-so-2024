#ifndef INTERFACE_PROTO_H
#define INTERFACE_PROTO_H

#include <proto/proto.h>

typedef struct
{
    char *name;
    char *type;
} t_interface_new_msg;

typedef struct
{
    uint32_t pid;
    char *interface_name;
} t_interface_io_done_msg;

typedef struct
{
    uint32_t work_units;
} t_interface_io_gen_sleep_msg;

typedef struct
{
    uint32_t page_number;
    uint32_t offset;
    uint32_t size;
} t_interface_io_stdin_read_msg;

typedef struct
{
    uint32_t page_number;
    uint32_t offset;
    uint32_t size;
} t_interface_io_stdout_write_msg;

// =========== GENERAL =============
int interface_send_io_error(int fd, char *interface_name, uint32_t pid,uint8_t error_code);
int interface_send_io_done(int fd, char *interface_name, uint32_t pid);
void interface_decode_io_done(t_buffer *buffer, t_interface_io_done_msg *msg);
void interface_destroy_io_done(t_interface_io_done_msg *msg);
int interface_send_new_interface(int fd, uint32_t pid, uint32_t work_units);
void interface_decode_new(t_buffer *buffer, t_interface_new_msg *interface);
void interface_destroy_new(t_interface_new_msg *interface);

// =========== IO GEN SLEEP =============
int interface_send_io_gen_sleep(int fd, uint32_t pid, uint32_t work_units);
void interface_decode_io_gen_sleep(t_buffer *buffer, t_interface_io_gen_sleep_msg *msg);
void interface_destroy_io_gen_sleep(t_interface_io_gen_sleep_msg *msg);

// =========== IO STDIN READ =============
int interface_send_io_stdin_read(int fd, uint32_t pid, uint32_t page_number, uint32_t offset, uint32_t size);
void interface_decode_io_stdin_read(t_buffer *buffer, t_interface_io_stdin_read_msg *msg);
void interface_destroy_io_stdin_read(t_interface_io_stdin_read_msg *msg);

// =========== IO STDOUT WRITE  =============
int interface_send_io_stdout_write(int fd, uint32_t pid, uint32_t page_number, uint32_t offset, uint32_t size);
void interface_decode_io_stdout_write(t_buffer *buffer, t_interface_io_stdout_write_msg *msg);
void interface_destroy_io_stdout_write(t_interface_io_stdout_write_msg *msg);

#endif