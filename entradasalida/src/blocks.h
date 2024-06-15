#ifndef BLOCKS_H
#define BLOCKS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <config.h>
#include <string.h>
#include <commons/log.h>
#include <sys/mman.h>
#include <utils.h>

void load_blocks(t_log *logger);
void close_blocks(void);

#endif