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
    gtree_node_t * gtree_node = (gtree_node_t *) kmalloc(sizeof(gtree_node_t));
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

// void gtree_remove_node_internal(gtree_t * gtree,gtree_node_t * gtree_node){
//     if(gtree_node->first_child){
//         gtree_remove_node_internal(gtree,gtree_node->first_child);  
//         kfree(gtree_node->first_child);
//         gtree_node->first_child = NULL; 

//     }

//     if(gtree_node->next_subling){
//         gtree_remove_node_internal(gtree,gtree_node->next_subling);   
//         kfree(gtree_node->next_subling);
//         gtree_node->next_subling = NULL;
//     }
// }

void remove_node(gtree_node_t * gtree_node){
        kfree(gtree_node);
        gtree_node = NULL;
}
void gtree_hierachy_execute(gtree_node_t * gtree_node, void(*func)(gtree_node_t *)){
    if(gtree_node->first_child){
        gtree_hierachy_execute(gtree_node->first_child,func);
        func(gtree_node->first_child);
    }

    if(gtree_node->next_subling){
        gtree_hierachy_execute(gtree_node->next_subling,func);
        func(gtree_node->next_subling);
    }
}


void gtree_remove_node_internal(gtree_t * gtree,gtree_node_t * gtree_node){
    
}



void gtree_remove_sub(gtree_t * gtree,gtree_node_t * gtree_node){
    if(!gtree_node->first_child)
    {
        return;
    }
    gtree_hierachy_execute(gtree_node->first_child,remove_node);
    gtree_node->first_child = NULL;
}



// gtree_node_t * gtree_find(gtree_node_t * base_node, int(*match)(gtree_node_t *)){
//     if(!base_node->first_child){
//         return NULL;
//     }


// }
