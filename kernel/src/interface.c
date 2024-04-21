#include <interface.h>

t_dictionary *interface_dictionary;

void interface_init()
{
    interface_dictionary = dictionary_create();
}

void interface_add(t_interface *interface)
{
    dictionary_put(interface_dictionary, interface->name, interface);
}

t_interface *interface_get(char *name)
{
    return dictionary_get(interface_dictionary, name);
}

int interface_is_connected(t_interface *interface)
{
    return socket_isConnected(interface->fd);
}

int interface_can_run_instruction(t_interface *interface, uint8_t instruction_to_run)
{
    switch (instruction_to_run)
    {
    case IO_GEN_SLEEP:
        return interface->type == "GENERICA";
    case IO_STD_IN_READ:
        return interface->type == "STDIN";
    case IO_STD_OUT_WRITE:
        return interface->type == "STDOUT";
    // the rest of instructions correspond to the file system
    default:
        return interface->type == "DIALFS";
    }
}

t_interface *interface_validate(char *name, uint8_t instruction_to_run)
{
    t_interface *interface = interface_get(name);

    // check if exists
    if (interface == NULL)
        return NULL;

    if (!interface_is_connected(name))
    {
        socket_freeConn(interface->fd);
        return NULL;
    };

    if (!interface_can_run_instruction(interface, instruction_to_run))
        return NULL;

    return interface;
}

void interface_destroy(t_interface *interface)
{
    dictionary_remove_and_destroy(interface_dictionary, interface->name, interface);
}

/**
 *
 * WRITE HERE THE CORRESPONDING FUNCTIONS TO SEND AND DECODE MESSAGES
 *
 */

void interface_decode_new(t_buffer *buffer, t_interface *interface)
{
    interface->name = packet_getString(buffer);
    interface->type = packet_getString(buffer);
}

int interface_send_io_gen_sleep(int fd, uint32_t pid, uint32_t work_units)
{
    t_packet *packet = packet_new(IO_GEN_SLEEP);
    // todo get the current exec instruction pid
    packet_addUInt32(packet, pid);
    packet_addUInt32(packet, work_units);
    int res = packet_send(packet, fd);
    free(packet);
    return res;
}