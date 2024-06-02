#include <prompt.h>

char *prompt(int size)
{
    char buffer[MAX_LIMIT] = {'\0'};
    printf("press ENTER to send a message\n");
    fread(buffer,size,sizeof(char),stdin);
    if (buffer[strlen(buffer) - 1] == '\n')
        buffer[strlen(buffer) - 1] = '\0';
    return strdup(buffer);
}