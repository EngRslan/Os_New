#ifndef LIST_H
#define LIST_H
#include <kernel/types.h>

#define foreach(item,list) for(list_node_t * item = list->head;item != NULL;item=item->next)

typedef struct list_node{
  struct list_node * next;
  struct list_node * previous;
  ptr_t value_ptr;
} list_node_t;

typedef struct
{
    list_node_t * head;
    list_node_t * tail;
    uint32_t count;
} list_t;


list_t * list_create();
void list_add_item(list_t * list_ptr, ptr_t value_ptr);
int list_remove_item(list_t * list_ptr, ptr_t value_ptr);
int list_count(list_t * list_ptr);
void list_destroy(list_t * list_ptr);
#endif