#include <interface.h>

t_dictionary *interface_dictionary;

static void interface_destroyer(void *_interface);

void interface_init(void)
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

t_interface *interface_get_by_fd(int fd)
{
    bool closure(void *elem)
    {
        t_interface *interface = (t_interface *)elem;
        return interface->fd == fd;
    }
    t_list* list = dictionary_elements(interface_dictionary);
    t_interface* tmp = list_find(list,closure);
    list_destroy(list);
    return tmp;

}

void destroy_interface_dictionary(void)
{
    dictionary_destroy_and_destroy_elements(interface_dictionary, interface_destroyer);
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
        return strcmp(interface->type, "GENERICA") == 0;
    case IO_STD_IN_READ:
        return strcmp(interface->type, "STDIN") == 0;
    case IO_STD_OUT_WRITE:
        return strcmp(interface->type, "STDOUT") == 0;
    // the rest of instructions correspond to the file system
    default:
        return strcmp(interface->type, "DIALFS") == 0;
    }
}

t_interface *interface_validate(char *name, uint8_t instruction_to_run)
{
    t_interface *interface = interface_get(name);

    // check if exists
    if (interface == NULL)
        return NULL;

    // comento por ahora pero por si las dudas dejo
    // if (!interface_is_connected(interface))
    // {
    //     interface_destroy(interface);
    //     socket_freeConn(interface->fd);
    //     return NULL;
    // }

    if (!interface_can_run_instruction(interface, instruction_to_run))
        return NULL;

    return interface;
}

void interface_destroy(t_interface *interface)
{
    // aca podriamos mandar a exit los procesos de la blocked queue de la interfaz
    dictionary_remove_and_destroy(interface_dictionary, interface->name,interface_destroyer);
}

static void interface_destroyer(void *_interface)
{
    t_interface *interface = (t_interface *)_interface;
    free(interface->name);
    free(interface->type);
    free(interface);
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

void interface_decode_io_gen_sleep(t_buffer *buffer, struct req_io_gen_sleep *params)
{
    params->interface_name = packet_getString(buffer);
    params->work_units = packet_getUInt32(buffer);
}

void interface_destroy_io_gen_sleep(struct req_io_gen_sleep *params)
{
    free(params->interface_name);
    free(params);
}

int interface_send_io_gen_sleep(int fd, uint32_t pid, uint32_t work_units)
{
    t_packet *packet = packet_new(IO_GEN_SLEEP);
    // todo get the current exec instruction pid
    packet_addUInt32(packet, pid);
    packet_addUInt32(packet, work_units);
    int res = packet_send(packet, fd);
    packet_free(packet);
    return res;
}
