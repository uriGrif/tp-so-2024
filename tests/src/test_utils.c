#include<stdio.h>
#include<utils/utlis.h>
#include<stdlib.h>
#include<cspecs/cspec.h>


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
}