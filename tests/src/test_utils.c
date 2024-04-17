#include<stdio.h>
#include<utils/utlis.h>
#include<stdlib.h>
#include<cspecs/cspec.h>
#include<commons/collections/list.h>


context(utils){
    describe("str_arr_to_string"){
        char * prueba[] = {"hola","me","llamo","juan",NULL};
        char* vacio[] = {NULL};
        // SI O SI CON NULL AL FINAL
        it("me deberia dar [hola,me,llamo,juan]"){
            char * result = string_arr_as_string(prueba);
            should_string(result) be equal to("[hola,me,llamo,juan]");
            free(result);
        } end

        it("le paso null"){
            char * result = string_arr_as_string(NULL);
            should_ptr(result) be equal to (NULL);
        } end

        it("le paso un arrgeglo vacio, asumo que me da []"){
            char * result = string_arr_as_string(vacio);
            should_string(result) be equal to("[]");
            free(result);
        } end
    } end

    describe("file_get_list_of_lines"){
        FILE *f;
        before{
            f = fopen("ejemplito.txt","w+");
        } end

        after{
             remove("ejemplito.txt");
        } end


        it("Archivo con 5 lineas"){
            fprintf(f,"Hola\nMe\nLlamo\nJuan\n:D");
            fclose(f);
            t_list* lines = file_get_list_of_lines("ejemplito.txt");
            should_int(list_size(lines)) be equal to(5);
            should_string(list_get(lines,0)) be equal to("Hola");
            should_string(list_get(lines,4)) be equal to(":D");
            list_destroy_and_destroy_elements(lines,free);
        } end

        it("archivo no encontrado deberia devolver lista vacia"){
            t_list* lines = file_get_list_of_lines("no_existe.txt");
            should_bool(list_is_empty(lines)) be equal to(true);
            list_destroy(lines);
        } end

        it("archivo vacio deberia devolver lista vacia"){
            fclose(f);
            t_list* lines = file_get_list_of_lines("ejemplito.txt");
            should_bool(list_is_empty(lines)) be equal to(true);
            list_destroy(lines);
        } end
    } end

    describe("file_get_nth_line"){
         FILE *f;
        before{
            f = fopen("ejemplito2.txt","w+");
        } end

        after{
             remove("ejemplito2.txt");
        } end

        it("archivo con 5 lineas le pido la tercera que deberia ser jorge"){
            FILE * f = fopen("otro.txt","w+");
            fprintf(f,"hola\nsoy\njorge\ntengo\n5 anios");
            fclose(f);
            char * res = file_get_nth_line("otro.txt",2);
            should_string(res) be equal to ("jorge");
            free(res);
        } end

        it("archivo no encontrado devuelve NULL"){
            fclose(f);
            char* res = file_get_nth_line("no_existe.xtx",50);
            should_ptr(res) be equal to (NULL);
        } end

        it("le pido linea que excede la cantidad del archivo deberia devovler NULL"){
            fprintf(f,"hola\nsoy\njorge\ntengo\n5 anios");
            fclose(f);
            char * res = file_get_nth_line("ejemplito2.txt",20);
            should_ptr(res) be equal to (NULL);
            
        } end

        it("pido la primer linea deberia devolver hola"){
            fprintf(f,"hola\nsoy\njorge\ntengo\n5 anios");
            fclose(f);
            char * res = file_get_nth_line("ejemplito2.txt",0);
            should_string(res) be equal to("hola");
            free(res);
        } end
        it("le pido la ultima linea, deberia devolver 5 anios"){
            fprintf(f,"hola\nsoy\njorge\ntengo\n5 anios");
            fclose(f);
            char * res = file_get_nth_line("ejemplito2.txt",4);
            should_string(res) be equal to("5 anios");
            free(res);
        } end

        it("archivo vacio deberia devolver NULL"){
            fclose(f);
            char * res=file_get_nth_line("ejemplito2.txt",0);
            should_ptr(res) be equal to(NULL);

        } end
    } end
}

