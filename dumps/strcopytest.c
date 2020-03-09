#include <stdio.h>
#include <string.h>


typedef struct t
{
    char x[20];
} str;

int main(int argc, char const *argv[])
{
    str s;
    strcpy(s.x, "hello");
    printf("%s\n", s.x);

    str s1;
    s1 = s;
    printf("%s\n", s1.x);
    return 0;
}
