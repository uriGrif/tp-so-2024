#include <prompt.h>

char *prompt(int size)
{
    char buffer[MAX_LIMIT];
    printf("press ENTER to send a message\n");
    fgets(buffer, size, stdin);
    if (buffer[strlen(buffer) - 1] == '\n')
        buffer[strlen(buffer) - 1] = '\0';
    return strdup(buffer);
}