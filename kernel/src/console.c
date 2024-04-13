#include <console.h>

// este array para readline

char *COMMAND_NAMES[] = {
    "EJECUTAR_SCRIPT",
    "INICIAR_PROCESO",
    "FINALIZAR_PROCESO",
    "DETENER_PLANIFICACION",
    "INICIAR_PLANIFICACION",
    "MULTIPROGRAMACION",
    "PROCESO_ESTADO",
    NULL};

/* When non-zero, this global means the user is done using this program. */
int done;

void do_nothing(char *algo)
{
    printf("me llego un comando %s\n", algo);
}

t_command COMMANDS[] = {
    {"EJECUTAR_SCRIPT", do_nothing, "Execute a script", 1},
    {"INICIAR_PROCESO", do_nothing, "Execute a script", 1},
    {"FINALIZAR_PROCESO", do_nothing, "Execute a script", 1},
    {"DETENER_PLANIFICACION", do_nothing, "Execute a script", 0},
    {"INICIAR_PLANIFICACION", do_nothing, "Execute a script", 0},
    {"MULTIPROGRAMACION", do_nothing, "Execute a script", 1},
    {"PROCESO_ESTADO", do_nothing, "Execute a script", 0},
    {NULL, NULL}};

char *generator(const char *text, int state)
{
    static int idx;
    char *ret;

    if (!state)
        idx = 0;

    while ((ret = COMMANDS[idx++].name))
    {
        if (strstr(ret, text) != NULL)
            return strdup(ret);
    }

    return NULL;
}

char **completion(const char *text, int start, int end)
{
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, generator);
}

void parse_command(char *string)
{
    char **command_split = string_n_split(string, 2, " ");
    char *name = command_split[0];
    char *param = command_split[1];

    // check for names
    for (int i = 0; COMMANDS[i].name != NULL; i++)
    {
        if (!strcmp(name, COMMANDS[i].name))
        {
            if (COMMANDS[i].expects_parameter && param == NULL)
            {
                printf("esperaba un parametro, ");
                break;
            }
            COMMANDS[i].func(param);
            string_array_destroy(command_split);
            return;
        }
    }

    printf("comando invalido\n");
    string_array_destroy(command_split);
}

void start_console(void)
{

    rl_bind_key('\t', rl_complete);
    rl_attempted_completion_function = completion;

    char *line;

    for (;;)
    {
        line = readline(">: ");

        if (!line)
            break;

        /* Remove leading and trailing whitespace from the line.
           Then, if there is anything left, add it to the history list
           and execute it. */
        string_trim(&line);

        if (*line)
        {
            add_history(line);
            parse_command(line);
        }

        free(line);
    }
}