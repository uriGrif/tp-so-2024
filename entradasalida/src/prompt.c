#include <prompt.h>

void prompt(int size, char *string)
{
    printf("press ENTER to send a message\n");
    scanf("%s", string);
}