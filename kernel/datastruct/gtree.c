#include <kernel/mem/kheap.h>
#include <kernel/types.h>
#include <kernel/datastruct/gtree.h>

gtree_node_t * gtree_create_child(gtree_node_t * parent_node,int32_t value){
    if(!parent_node)
        return NULL;
    gtree_node_t * gtree_node = (gtree_node_t *) kmalloc(sizeof(gtree_node_t));
    gtree_node->next_subling = parent_node->first_child;
    parent_node->first_child = gtree_node;
    gtree_node->value = value;
    return gtree_node;
}

gtree_t * gtree_create(int32_t root_value){
    gtree_t * gtree = kmalloc(sizeof(gtree_t));
    gtree_node_t * root_node = (gtree_node_t *) kmalloc(sizeof(gtree_node_t));
    root_node->next_subling = NULL;
    root_node->first_child = NULL;
    root_node->value = root_value;
    gtree->root = root_node;
    return gtree;
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

void remove_node(gtree_node_t * gtree_node,int _){
        kfree(gtree_node);
        gtree_node = NULL;
}

void gtree_descendant_exec_rec(gtree_node_t * gtree_node, void(*func)(gtree_node_t *,int depth),int32_t depth_from,int32_t is_reverse){
    if(is_reverse<=0){
        func(gtree_node,depth_from);
    }
    if(gtree_node->first_child){
        depth_from++;
        gtree_descendant_exec_rec(gtree_node->first_child,func,depth_from,is_reverse);
        depth_from--;
    }
    if(gtree_node->next_subling){
        gtree_descendant_exec_rec(gtree_node->next_subling,func,depth_from,is_reverse);
    }

    if(is_reverse>0){
        func(gtree_node,depth_from);
    }
}


gtree_node_t * gtree_descendant_query_rec(gtree_node_t * gtree_node, int(*func)(gtree_node_t *)){
    gtree_node_t * cur = NULL;

    if(func(gtree_node)>0)
       return gtree_node;

    if(gtree_node->first_child){
        cur = gtree_descendant_query_rec(gtree_node->first_child,func);
        if(cur)
            return cur;
    }

    if(gtree_node->next_subling){
        cur = gtree_descendant_query_rec(gtree_node->next_subling,func);
        if(cur)
            return cur;
    }

    return NULL;
}

void gtree_remove_descendant(gtree_t * gtree,gtree_node_t * gtree_node){
    if(!gtree_node->first_child)
    {
        return;
    }
    gtree_descendant_exec(gtree_node->first_child,remove_node,1);
    gtree_node->first_child = NULL;
}


// gtree_node_t * gtree_find(gtree_node_t * base_node, int(*match)(gtree_node_t *)){
//     if(!base_node->first_child){
//         return NULL;
//     }
//         if(gtree_hierachy_execute(base_node,match)==0){

//         }
// }

// void gtree_hierachy_execute(gtree_node_t * gtree_node, void(*func)(gtree_node_t *,int depth)){
//     if(!gtree_node)
//         return;
//     int depth_from = 0;
//     func(gtree_node,depth_from);
//     if(!gtree_node->first_child)
//         return;
//     depth_from ++;
//     gtree_hierachy_execute_rec(gtree_node->first_child, func , depth_from,0);
//     depth_from --;
// }
void gtree_descendant_exec(gtree_node_t * gtree_node, void(*func)(gtree_node_t *,int depth),int32_t is_reverse){
    if(!gtree_node)
        return;
    if(!gtree_node->first_child)
        return;
    gtree_descendant_exec_rec(gtree_node->first_child, func , 0,is_reverse);
}
gtree_node_t * gtree_descendant_query(gtree_node_t * gtree_node, int(*func)(gtree_node_t *)){
   
    if(!gtree_node->first_child)
        return NULL;

    return gtree_descendant_query_rec(gtree_node->first_child,func);
}