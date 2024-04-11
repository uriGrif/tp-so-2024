#include<utils/utlis.h>

char * string_arr_as_string(char ** string_arr){
    char * result = strdup("[");
    for(int i =0 ; string_arr[i] != NULL; i++){
        string_append(&result,string_arr[i]);
        if(string_arr[i + 1])
            string_append(&result,",");
    }   
    string_append(&result,"]");
    return result;
}