#include <stdio.h>

#define NodePtrList Node * []

typedef struct Node
{
    char type[20];
    
    int _numnodes;
    struct Node* ptrlist[10];
} Node;

Node* make_node(char *type, Node* *list)
{
    int i = 0;
    while(list[i] != NULL)
    {
        i++;
    }
    printf("%d\n", i);
}

int main(int argc, char const *argv[])
{
    Node a, b, c;
    // make_node("hi", (Node **) {NULL});  // doesn't work - segfaults
    make_node("hi", (Node * []) {&a, &b, &c, NULL});
    make_node("hi", (NodePtrList) {&a, &b, &c, NULL});


    return 0;
}
