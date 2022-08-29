#ifndef TREE_H
#define TREE_H
#include <kernel/types.h>
#include "list.h"

typedef struct tree_node
{
    ptr_t value ;
    list_t * children;
    struct tree_node * parent;

}tree_node_t ;

typedef struct
{
    tree_node_t * root;
} tree_t;
tree_t * tree_create();
#endif