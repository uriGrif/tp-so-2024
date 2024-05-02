#include <prompt.h>

void prompt(int size, char *string)
{
    printf("press ENTER to send a message\n");
    fgets(string, size, stdin);
}