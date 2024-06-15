#include <blocks.h>

static void* blocks_mmap;

static void create_blocks(t_log* logger){
    char* blocks_name = mount_pathbase("bloques.dat");
    FILE* f = fopen(blocks_name,"wb+");
    uint8_t* tmp = calloc(cfg_io->block_count*cfg_io->block_size,sizeof(uint8_t));
    fwrite(tmp, sizeof(uint8_t), cfg_io->block_count*cfg_io->block_size,f);
    fclose(f);
    free(blocks_name);
    free(tmp);
    log_debug(logger,"se ha creado un nuevo archivo de bloques.dat");
}

void load_blocks(t_log* logger){
    char* blocks_name = mount_pathbase("bloques.dat");
    FILE* f = fopen(blocks_name,"rb+");
    if(!f){
        create_blocks(logger);
        f = fopen(blocks_name,"rb+");
    }

    int blocks_fd = fileno(f);

    blocks_mmap = mmap(0,cfg_io->block_count*cfg_io->block_size, PROT_READ|PROT_WRITE,MAP_SHARED, blocks_fd, 0);

    free(blocks_name);
    
    fclose(f);
}


void close_blocks(void){
    munmap(blocks_mmap,cfg_io->block_count*cfg_io->block_size);
}

