#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void init_process(char *path);
void end_process(char *pid);
void stop_scheduler(char *);
void start_scheduler(char *);
void multiprogramming(char *value);
void list_processes_by_state(char *);

#endif