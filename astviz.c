#include <stdio.h>
#include "ast.h"

void bst_print_dot_null(int key, int nullcount, FILE* stream)
{
    fprintf(stream, "    null%d [shape=point];\n", nullcount);
    fprintf(stream, "    %d -> null%d;\n", key, nullcount);
}

void bst_print_dot_aux(Node* node, FILE* stream)
{
    static int nullcount = 0;

    if (node->left)
    {
        fprintf(stream, "    %d -> %d;\n", node->key, node->left->key);
        bst_print_dot_aux(node->left, stream);
    }
    else
        bst_print_dot_null(node->key, nullcount++, stream);

    if (node->right)
    {
        fprintf(stream, "    %d -> %d;\n", node->key, node->right->key);
        bst_print_dot_aux(node->right, stream);
    }
    else
        bst_print_dot_null(node->key, nullcount++, stream);
}

void tree_print_dot(Node* root, FILE* stream)
{
    fprintf(stream, "digraph AST {\n");
    fprintf(stream, "    node [fontname=\"Arial\"];\n");

    if (!root)
        fprintf(stream, "\n");
    else
        bst_print_dot_aux(root, stream);

    fprintf(stream, "}\n");
}