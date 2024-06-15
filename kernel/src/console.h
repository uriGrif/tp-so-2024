#ifndef CONSOLE_H
#define CONSOLE_H

#define SCRIPTS_ABSOLUTE_PATH "/home/utnso/tp-2024-1c-fossil/kernel/scripts"

#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <commons/string.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include<utils/utlis.h>
#include<commons/log.h>
#include <commands.h>

typedef struct
{
    char *name;
    void (*func)(char *,t_log* logger);
    char *doc;
    int expects_parameter;
} t_command;

#define BUFFER_MAX_LENGTH 100

void start_console(t_log* logger);

#endif