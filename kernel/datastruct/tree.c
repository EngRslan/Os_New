#include <kernel/types.h>
#include <kernel/datastruct/list.h>
#include <kernel/datastruct/tree.h>
#include <kernel/mem/kheap.h>


tree_t * tree_create(){
    tree_t * tree_ptr = (tree_node_t *)kmalloc(sizeof(tree_t));
    return tree_ptr;
}

tree_node_t * tree_create_node(ptr_t value_ptr){
    tree_node_t * tree_node = (tree_node_t *)kmalloc(sizeof(tree_node_t));
    tree_node->value = value_ptr;
    tree_node->children = list_create();
}

tree_node_t * tree_insert_node(tree_t * tree_ptr,tree_node_t * node_ptr,ptr_t value_ptr){
    tree_node_t * node = tree_create_node(value_ptr);
    if(!tree_ptr->root){
        tree_ptr->root = node;
        return node;
    }
    
    list_add_item(node_ptr->children,node);
    return node;

}

