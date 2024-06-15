#include <init_fs.h>

void load_fs(t_log *logger) {
    load_blocks(logger);
    load_bitmap(logger);
    init_fcbs();
}


void close_fs(void){
    close_blocks();
    close_bitmap();
}