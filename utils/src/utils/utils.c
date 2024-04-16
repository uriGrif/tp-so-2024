#include <utils/utlis.h>

char *string_arr_as_string(char **string_arr)
{
    if (!string_arr)
        return NULL;

    char *result = strdup("[");
    for (int i = 0; string_arr[i] != NULL; i++)
    {
        string_append(&result, string_arr[i]);
        if (string_arr[i + 1])
            string_append(&result, ",");
    }
    string_append(&result, "]");
    return result;
}

t_list *get_list_of_lines(char *file_path)
{
    char buffer[BUFFER_MAX_LENGTH];
    t_list *list = list_create();
    // asumo por ahora que el archivo no es binario
    FILE *f = fopen(file_path, "r");
    if (!f)
        return list;
    while (fgets(buffer, BUFFER_MAX_LENGTH, f))
    {
        if ('\n' == buffer[strlen(buffer) - 1])
            buffer[strlen(buffer) - 1] = '\0';

        list_add(list, strdup(buffer));
    }
    fclose(f);
    return list;
}