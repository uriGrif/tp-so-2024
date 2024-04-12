#include<stdio.h>
#include<stdint.h>
#include <proto/proto.h>
#include<cspecs/cspec.h>


typedef struct {
    char* tipo;
    uint8_t cant_patas;
} t_animal;


static void packet_add_animal(t_packet* packet,t_animal* animal){
    packet_addString(packet,animal->tipo);
    packet_add_uint8(packet,animal->cant_patas);
}

static t_animal* packet_get_animal(t_buffer* buffer){
    t_animal* res = malloc(sizeof(t_animal));
    res->tipo = packet_getString(buffer);
    // MAS EFICIENTE QUE USAR LA OTRA
    packet_get(buffer,&res->cant_patas,sizeof(uint8_t));
    return res;
}


context(test_protocol){

    t_packet* packet;

    describe("protocolo: mando numeros"){
        before {
            packet = packet_new(0);
            packet_addUInt32(packet,100);
            packet_add_uint8(packet,50);
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

    } end

    describe("protocolo: mando un animal"){
        before{
            packet = packet_new(0);
            t_animal* ani = malloc(sizeof(t_animal));
            ani->tipo = strdup("leon");
            ani->cant_patas = 4;
            packet_add_animal(packet,ani);
            free(ani->tipo);
            free(ani);
        } end

        after{ 
            packet_free(packet);
        } end

        it("buffer size"){
            should_int(packet->buffer->size) be equal to(10);
        } end

        it("reviso que me llegue un animal leon con 4 patas"){
            t_animal* res = packet_get_animal(packet->buffer);
            should_string(res->tipo) be equal to("leon");
            should_int(res->cant_patas) be equal to(4);
            free(res->tipo);
            free(res);
        } end
    } end

    describe("protocolo: mando un array de strings"){

        char* arr[] = {"uno","dos","tres","cuatro",NULL}; 

        before{ 
            packet = packet_new(0);
            // SI NO LE PONES NULL TIRA SEGFAULT ACA
            packet_add_string_arr(packet,arr);
        } end
        after{
            packet_free(packet);
        } end

        it("test buffer size"){
            // es un string del tipo "[uno,dos,tres,cuatro]"
            should_int(packet->buffer->size) be equal to(26);
        } end

        it("test de que me haya llegado uno dos tres cuatro"){
            int i =0;

            void check_string(char * str){
                should_string(str) be equal to(arr[i++]);
                free(str);
            }

            char ** res = packet_get_string_arr(packet->buffer);
            string_iterate_lines(res,check_string);
            free(res);
        } end

    } end
}