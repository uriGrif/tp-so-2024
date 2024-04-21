#include <commons/collections/dictionary.h>
#include <proto/proto.h>
#include <sockets/sockets.h>

enum interface_types
{
    GENERIC,
    STDIN,
    STDOUT,
    DIALFS
};

typedef struct
{
    char *name;
    char *type;
    int fd;
} t_interface;

extern t_dictionary *interface_dictionary;

void interface_decode_new(t_buffer *buffer, t_interface *interface);
void interface_decode_io_gen_sleep(t_buffer *buffer, t_interface *interface);
void interface_init();
void interface_add(t_interface *interface);
t_interface *interface_validate(char *name, uint8_t instruction_to_run);
t_interface *interface_get(char *name);
int interface_is_connected(char *name);
int interface_can_run_instruction(t_interface *interface, uint8_t instruction_to_run);
void interface_destroy(t_interface *interface);
char *interface_get_type_name(enum interface_types type);

// message senders
int interface_send_io_gen_sleep(int fd, uint32_t pid, uint32_t work_units);