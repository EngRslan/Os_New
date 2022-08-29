#include <kernel/mem/kheap.h>
#include <kernel/types.h>
#include <kernel/datastruct/gtree.h>

gtree_t * gtree_create(int32_t root_value){
    gtree_t * gtree = kmalloc(sizeof(gtree_t));
    gtree->root = NULL;
    gtree->value = root_value;
    return gtree;
}

gtree_node_t * gtree_create_node(gtree_t * gtree,gtree_node_t * parent_node,int32_t value){
    gtree_node_t * gtree_node = (gtree_node_t *) kmalloc(sizeof(parent_node));
    gtree_node->first_child = NULL;
    gtree_node->next_subling = NULL;
    gtree_node->value = value;

    if(parent_node == NULL){
        gtree_node->next_subling = gtree->root;
        gtree->root = gtree_node;
    }else{
        gtree_node->next_subling = parent_node->first_child;
        parent_node->first_child = gtree_node;
    }
    return gtree_node;
}

void gtree_remove_node(gtree_t * gtree,gtree_node_t * parent_node,gtree_node_t * gtree_node){
    if(!gtree_node->first_child){
        if(parent_node == NULL){
            gtree->root = NULL;
        }else{
            parent_node->first_child = gtree_node->next_subling;
        }
        kfree(gtree_node);
        return;
    }

    gtree_remove_node(gtree,gtree_node,gtree_node->first_child);
}



