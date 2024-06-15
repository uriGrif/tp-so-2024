#ifndef INIT_FS_H
#define INIT_FS_H

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <blocks.h>
#include <bitmap_monitor.h>
#include <fcb.h>

void load_fs(t_log *logger);
void close_fs(void);

#endif