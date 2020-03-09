#include <stdio.h>
#include <string.h>

typedef union data
{
    int num_const;
    char str_const[5];
} data;

void datathing(data t)
{
    printf("%d\n", t.num_const);
    printf("%s\n", t.str_const);
}

int main(int argc, char const *argv[])
{
    datathing((data) 97);
    data x;
    strcpy(x.str_const, "abc");
    datathing(x);

    // datathing((data) "abcd");
    return 0;
}
