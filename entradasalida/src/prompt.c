#include <prompt.h>

char *prompt(int size)
{
    // int c;
    // while((c=getchar()!='\n') && c !=EOF);
    char *line;
    do
    {
        line = readline("press Enter to send a message\n");
        if (line && *line)
            break;
        free(line);
    } while (1);
    char* result = calloc(size+1,sizeof(char));
    strncpy(result,line,size);
    free(line);
    return result;
}