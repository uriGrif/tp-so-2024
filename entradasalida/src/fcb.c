#include <fcb.h>

static t_dictionary* metada_dictionary;

void init_fcbs(void){
    metada_dictionary = dictionary_create();
    // cargar todos los ya existentes
    DIR *d;
    struct dirent *dir;
    char *files_path = mount_pathbase("/files");
    d = opendir(files_path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            // printf("%s\n", dir->d_name);
            char* final_name = mount_pathbase_files(dir->d_name);
            dictionary_put(metada_dictionary, dir->d_name, config_create(final_name));
            free(final_name);
        }
        closedir(d);
    }
    free(files_path);
}

bool file_already_exists(char* file_name){
    return dictionary_get(metada_dictionary,file_name) != NULL;
}

void create_file(char* file_name){
    char* final_name = mount_pathbase_files(file_name);
    FILE* f = fopen(final_name,"wb+");
    fclose(f);
    t_config* config = config_create(final_name);
    // econtrar el bloque libre
    uint32_t first_block = find_first_free_block();

    char* first_block_str = string_itoa(first_block);

    config_set_value(config,"BLOQUE_INICIAL",first_block_str);
    config_set_value(config,"TAMANIO_ARCHIVO","0");
    config_save(config);

    dictionary_put(metada_dictionary,file_name,config);

    occupy_free_blocks(first_block, 1);

    free(final_name);
    free(first_block_str);
}

t_fcb* get_metadata(char* file_name){
    t_fcb* fcb = malloc(sizeof(t_fcb));
    t_config* config = dictionary_get(metada_dictionary,file_name);
    fcb->first_block = config_get_int_value(config,"BLOQUE_INICIAL");
    fcb->size = config_get_int_value(config,"TAMANIO_ARCHIVO");
    return fcb;
}

void delete_file(char* file_name){
    t_fcb* fcb = get_metadata(file_name);
    int blocks_occupied = ceil((double) fcb->size/cfg_io->block_size);
    int blocks_to_free = blocks_occupied ? blocks_occupied : 1;
    free_blocks(fcb->first_block,blocks_to_free);
    dictionary_remove_and_destroy(metada_dictionary,file_name,(void *)config_destroy);
    char* final_name = mount_pathbase_files(file_name);
    remove(final_name);
    free(final_name);
    free(fcb);
}