#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

Node* make_node(char *type, data value, Node* *list)
{
    Node *temp = (Node *) malloc(sizeof(Node));
    strcpy(temp->type, type);
    temp->value = value;

    int i = 0;
    while(list[i] != NULL)
    {
        temp->ptrlist[i] = list[i];
        i++;
    }

    temp->_numnodes = i;

    return temp;
}

void disp_node_details(Node *n)
{
    printf("Node Data: ");
    if(strcmp(n->type, "NUM_CONST") == 0)
    {
        printf("%d\n", n->value);
    }
}

void display_subtree(Node *n)
{
    if(n != NULL)
    {
        disp_node_details(n);
        for (int i = 0; i < n->_numnodes; i++)
        {
            display_subtree(n->ptrlist[i]);
        }
        
    }
}