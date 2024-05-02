#include "interface.h"

// =========== GENERAL =============
int interface_send_io_done(int fd, char *interface_name, uint32_t pid)
{
    t_packet *packet = packet_new(IO_DONE);
    packet_addString(packet, interface_name);
    packet_addUInt32(packet, pid);
    int res = packet_send(packet, fd);
    packet_free(packet);
    return res;
}

void interface_decode_io_done(t_buffer *buffer, t_interface_io_done_msg *msg)
{
    msg->interface_name = packet_getString(buffer);
    msg->pid = packet_getUInt32(buffer);
}

void interface_destroy_io_done(t_interface_io_done_msg *msg)
{
    free(msg->interface_name);
    free(msg);
}

int interface_send_new_interface(int fd, uint32_t pid, uint32_t work_units)
{
    t_packet *packet = packet_new(NEW_INTERFACE);
    packet_addUInt32(packet, pid);
    packet_addUInt32(packet, work_units);
    int res = packet_send(packet, fd);
    packet_free(packet);
    return res;
}

void interface_decode_new(t_buffer *buffer, t_interface_new_msg *interface)
{
    interface->name = packet_getString(buffer);
    interface->type = packet_getString(buffer);
}

void interface_destroy_new(t_interface_new_msg *interface)
{
    free(interface->name);
    free(interface->type);
    free(interface);
}

// =========== IO GEN SLEEP =============
int interface_send_io_gen_sleep(int fd, uint32_t pid, uint32_t work_units)
{
    t_packet *packet = packet_new(IO_GEN_SLEEP);
    packet_addUInt32(packet, pid);
    packet_addUInt32(packet, work_units);
    int res = packet_send(packet, fd);
    packet_free(packet);
    return res;
}

void interface_decode_io_gen_sleep(t_buffer *buffer, t_interface_io_gen_sleep_msg *msg)
{
    msg->work_units = packet_getUInt32(buffer);
}

void interface_destroy_io_gen_sleep(t_interface_io_gen_sleep_msg *msg)
{
    free(msg);
}

// =========== IO STDIN READ =============
int interface_send_io_stdin_read(int fd, uint32_t pid, char *address, uint32_t offset, uint32_t size)
{
    t_packet *packet = packet_new(IO_STDIN_READ);
    packet_addUInt32(packet, pid);
    packet_addString(packet, address);
    packet_addUInt32(packet, offset);
    packet_addUInt32(packet, size);
    int res = packet_send(packet, fd);
    packet_free(packet);
    return res;
}

void interface_decode_io_stdin_read(t_buffer *buffer, t_interface_io_stdin_read_msg *msg)
{
    msg->address = packet_getString(buffer);
    msg->offset = packet_getUInt32(buffer);
    msg->size = packet_getUInt32(buffer);
}

void interface_destroy_io_stdin_read(t_interface_io_stdin_read_msg *msg)
{
    free(msg->address);
    free(msg);
}

// =========== IO STDOUT WRITE  =============
int interface_send_io_stdout_write(int fd, uint32_t pid, char *address, uint32_t offset, uint32_t size)
{
    t_packet *packet = packet_new(IO_STDOUT_WRITE);
    packet_addUInt32(packet, pid);
    packet_addString(packet, address);
    packet_addUInt32(packet, offset);
    packet_addUInt32(packet, size);
    int res = packet_send(packet, fd);
    packet_free(packet);
    return res;
}

void interface_decode_io_stdout_write(t_buffer *buffer, t_interface_io_stdout_write_msg *msg)
{
    msg->address = packet_getString(buffer);
    msg->offset = packet_getUInt32(buffer);
    msg->size = packet_getUInt32(buffer);
}

void interface_destroy_io_stdout_write(t_interface_io_stdout_write_msg *msg)
{
    free(msg->address);
    free(msg);
}
