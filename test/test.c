#include <stdio.h>

int main(int argc, char const *argv[])
{
    char st[512];
    while(!feof(stdin))
    {
        fgets(st, 512, stdin);
        printf("%s\n", st);
        printf("feof: %d\n", feof(stdin));
    }
    return 0;
}
