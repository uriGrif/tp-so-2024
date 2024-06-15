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

t_list *file_get_list_of_lines(char *file_path)
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

char *file_get_nth_line(char *file_path, int n)
{
    char buffer[BUFFER_MAX_LENGTH];
    FILE *f = fopen(file_path, "r");
    if (!f)
        return NULL;
    uint8_t i = 0;
    while (fgets(buffer, BUFFER_MAX_LENGTH, f))
    {
        if (i == n)
        {
            // ENCONTRE
            if ('\n' == buffer[strlen(buffer) - 1])
                buffer[strlen(buffer) - 1] = '\0';
            fclose(f);
            return strdup(buffer);
        }

        i++;
    }
    // LINEA NO ENCONTRADA
    fclose(f);
    return NULL;
}


int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

int file_exists(char* file_path){
    return !access(file_path,F_OK);
}

char* mount_config_directory(char* module_name,char* config_path){
    return string_from_format("home/utnso/tp-2024-1c-fossil/%s/configs/%s",module_name,config_path);
}

long long current_timestamp(void){
    struct timeval te; 
    gettimeofday(&te, NULL); 
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    //printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
} 