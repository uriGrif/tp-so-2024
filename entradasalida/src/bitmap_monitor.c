#include <bitmap_monitor.h>
 
static t_bitarray* bitarray;


static void create_bitmap_file(t_log* logger){
    char* bitmap_name = mount_pathbase("bitmap.dat");
    FILE* f = fopen(bitmap_name,"wb+");
    int block_count_in_bytes = ceil(cfg_io->block_count / 8.0);
    char* raw_bitmap = calloc(block_count_in_bytes,sizeof(char));
    fwrite(raw_bitmap, sizeof(char), block_count_in_bytes,f);
    fclose(f);
    free(bitmap_name);
    free(raw_bitmap);
    log_debug(logger,"se ha creado un nuevo archivo de bitmap.dat");
}

void load_bitmap(t_log* logger){
    char* bitmap_name = mount_pathbase("bitmap.dat");
    FILE* f = fopen(bitmap_name,"rb+");
    if(!f){
        create_bitmap_file(logger);
        f = fopen(bitmap_name,"rb+");
    }

    int bitmap_fd = fileno(f);

    int block_count_in_bytes = ceil(cfg_io->block_count / 8.0);

    char *bitmap_mmap = mmap(0,block_count_in_bytes, PROT_READ|PROT_WRITE,MAP_SHARED, bitmap_fd, 0);

    bitarray = bitarray_create_with_mode(bitmap_mmap,block_count_in_bytes,LSB_FIRST);
    free(bitmap_name);
    
    fclose(f);
}

void close_bitmap(void){
    munmap(bitarray->bitarray,bitarray->size);
    bitarray_destroy(bitarray);
}

int find_first_free_block() {
    int block_index = 0;
    while(bitarray_test_bit(bitarray, block_index)) {
        block_index++;
    }
    return block_index;
}

uint32_t free_contiguous_blocks_from(int file_end){
    uint32_t count = 0;
    for(int i=file_end; !bitarray_test_bit(bitarray,i);i++)
        count++;
    return count;
}

void occupy_free_blocks(int from, int block_count){
    for(int i = 0; i < block_count; i++){
        bitarray_set_bit(bitarray,from + i);
    }
    msync(bitarray->bitarray,bitarray->size,MS_SYNC);
}

void free_blocks(int from, int block_count){
    for(int i = 0; i < block_count; i++){
        bitarray_clean_bit(bitarray,from + i);
    }
    msync(bitarray->bitarray,bitarray->size,MS_SYNC);
}

bool test_bit_from_bitmap(int bit) {
    return bitarray_test_bit(bitarray, bit);
}