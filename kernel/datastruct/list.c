#include <kernel/types.h>
#include <kernel/mem/kheap.h>
#include <kernel/datastruct/list.h>



//internal
list_node_t * list_create_node(list_t * list_ptr){
    list_node_t * list_node = kmalloc(sizeof(list_node_t));
    list_node->next = NULL;
    list_node->previous = list_ptr->tail;
    if(list_ptr->tail){
        list_ptr->tail->next = list_node;
    }
    list_ptr->tail = list_node;
    if(!list_ptr->head){
        list_ptr->head = list_node;
    }
    list_ptr->count ++;
    return list_node;
}

//internal
list_node_t * list_find_node(list_t * list_ptr, ptr_t value){
    
    foreach(list_node,list_ptr){
        if(list_node->value_ptr == value){
            return list_node;
        }
        list_node = list_node->next;
    }
    // list_node_t * list_node = list_ptr->head;
    // while (list_node->next)
    // {
    //     if(list_node->value_ptr == value){
    //         return list_node;
    //     }
    //     list_node = list_node->next;
    // }

    return NULL;
}

// internal
void list_remove_node(list_t * list_ptr, list_node_t * node_ptr){
    if(node_ptr->previous){ 
        node_ptr->previous->next = node_ptr->next;
    }else{ // First Node
        list_ptr->head = node_ptr->next;
    }

    if(node_ptr->next){
        node_ptr->next->previous = node_ptr->previous;
    } else { // Last node
        list_ptr->tail = node_ptr->previous;
    }
    kfree((ptr_t)node_ptr);
    list_ptr->count--;
}


list_t * list_create(){
    ptr_t list = kmalloc(sizeof(list_t));
    return (list_t *)list;
}

void list_add_item(list_t * list_ptr, ptr_t value_ptr){
    list_node_t * list_node = list_create_node(list_ptr);
    list_node->value_ptr = value_ptr;
}

int list_remove_item(list_t * list_ptr, ptr_t value_ptr){
    list_node_t * list_node = list_find_node(list_ptr,value_ptr);
    if(list_node){
        list_remove_node(list_ptr, list_node);
        return 1;
    }
    else
    {
        return 0;
    }
}


int list_count(list_t * list_ptr){
    return list_ptr->count;
}

void list_destroy(list_t * list_ptr){
    foreach(item,list_ptr){
        list_remove_node(list_ptr,item);
    }
    kfree((ptr_t)list_ptr);
}