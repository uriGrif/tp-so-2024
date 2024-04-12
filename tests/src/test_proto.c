#include <stdio.h>
#include <stdint.h>
#include <proto/proto.h>
#include <semaphore.h>
#include <unistd.h>
#include <sysexits.h>
#include <sys/mman.h>
#include <cspecs/cspec.h>

#define PUERTO_INICIAL 5000

char *proximo_puerto()
{
    static int numero_puerto = PUERTO_INICIAL;
    char *result = malloc(sizeof(int) + 1);
    sprintf(result, "%d", numero_puerto);
    numero_puerto++;
    return result;
}

static int fd_server;
static int fd_client;

void generar_conexiones()
{
    char *port = proximo_puerto();
    fd_server = socket_createTcpServer(NULL, port);
    if (fd_server == -1)
    {
        perror("error al levantar el server");
        free(port);
        return;
    }

    fd_client = socket_connectToServer(NULL, port);
    const int enable = 1;
    if (setsockopt(fd_client, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    if (fd_client == -1)
    {
        perror("error al crear el cliente");
        close(fd_server);
    }
    free(port);
}

void cerrar_conexiones()
{
    close(fd_client);
    close(fd_server);
}
typedef struct
{
    char *tipo;
    uint8_t cant_patas;
} t_animal;

static void packet_add_animal(t_packet *packet, t_animal *animal)
{
    packet_addString(packet, animal->tipo);
    packet_add_uint8(packet, animal->cant_patas);
}

static t_animal *packet_get_animal(t_buffer *buffer)
{
    t_animal *res = malloc(sizeof(t_animal));
    res->tipo = packet_getString(buffer);
    // MAS EFICIENTE QUE USAR LA OTRA
    packet_get(buffer, &res->cant_patas, sizeof(uint8_t));
    return res;
}

context(test_protocol)
{

    t_packet *packet;

    describe("protocolo: mando numeros"){
            before{
                packet = packet_new(0);
                packet_addUInt32(packet, 100);
                packet_add_uint8(packet, 50);
            } end

        after{
            packet_free(packet);
        } end

        it("test del size del buffer"){
            should_int(packet->buffer->size) be equal to(sizeof(uint32_t) + sizeof(uint8_t));
        } end

        it("reviso que lo que me llego sea 100 y 50 "){
            should_int(packet_getUInt32(packet->buffer)) be equal to(100);
            should_int(packet_get_uint8(packet->buffer)) be equal to(50);
        } end
    }end

    describe("protocolo: mando un animal"){
        before{
            packet = packet_new(0);
            t_animal *ani = malloc(sizeof(t_animal));
            ani->tipo = strdup("leon");
            ani->cant_patas = 4;
            packet_add_animal(packet, ani);
            free(ani->tipo);
            free(ani);
        }end

        after{
            packet_free(packet);
        } end

        it("buffer size"){
            should_int(packet->buffer->size) be equal to(10);
        }end

        it("reviso que me llegue un animal leon con 4 patas")
        {
            t_animal *res = packet_get_animal(packet->buffer);
            should_string(res->tipo) be equal to("leon");
            should_int(res->cant_patas) be equal to(4);
            free(res->tipo);
            free(res);
        }end
    }end

    describe("protocolo: mando un array de strings"){

        char *arr[] = {"uno", "dos", "tres", "cuatro", NULL};

        before{
            packet = packet_new(0);
            // SI NO LE PONES NULL TIRA SEGFAULT ACA
            packet_add_string_arr(packet, arr);
         } end

        after{
            packet_free(packet);
        }end

        it("test buffer size"){
            // es un string del tipo "[uno,dos,tres,cuatro]"
            should_int(packet->buffer->size) be equal to(26);
        }end

        it("test de que me haya llegado uno dos tres cuatro"){
        int i = 0;

        void check_string(char *str)
        {
            should_string(str) be equal to(arr[i++]);
            free(str);
        }

        char **res = packet_get_string_arr(packet->buffer);
        string_iterate_lines(res, check_string);
        free(res);
    }end
}end

    describe("protocolo: envio y recibo un packet por red"){
        before{
            generar_conexiones();
            packet = packet_new(5);
            packet_addUInt32(packet, 150);
        }end

        after{
            packet_free(packet);
            cerrar_conexiones();
        }end

        it("test del opcode size del buffer y contenido"){
            sem_t *sem_padre = (sem_t *)mmap(
                0,
                sizeof(sem_t),
                PROT_READ | PROT_WRITE,
                MAP_ANONYMOUS | MAP_SHARED,
                0,
                0);
            if ((void *)sem_padre == MAP_FAILED)
            {
                perror("mmap");
                exit(EX_OSERR);
            }
            sem_t *sem_hijo = (sem_t *)mmap(
                0,
                sizeof(sem_t),
                PROT_READ | PROT_WRITE,
                MAP_ANONYMOUS | MAP_SHARED,
                0,
                0);

            sem_init(sem_padre, 1, 1);
            sem_init(sem_hijo, 1, 0);
            pid_t pid = fork();

            if (pid == 0)
            {
                // ESTOY EN EL HIJO, LO USO DE CLIENTE
                sem_wait(sem_hijo);
                packet_send(packet, fd_client);
                sem_post(sem_padre);
                // exit(0);
            }
            else
            {
                sem_wait(sem_padre);
                int client_con = socket_acceptConns(fd_server);
                sem_post(sem_hijo);
                sem_wait(sem_padre);
                if (client_con == -1)
                    perror("error en la conexion");

                t_packet *p = packet_new(-1);
                if (packet_recv(client_con, p) == -1)
                    perror("error recibiendo el paquete");

                should_int(p->op_code) be equal to(5);
                should_int(p->buffer->size) be equal to(4);
                should_int(packet_getUInt32(p->buffer)) be equal to(150);
                packet_free(p);
                sem_destroy(sem_padre);
                sem_destroy(sem_hijo);
            }
    }end
 }end
}