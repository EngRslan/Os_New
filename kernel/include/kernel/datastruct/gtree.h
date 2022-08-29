#ifndef GTREE_H
#define GTREE_H

typedef struct gtree_node
{
  struct gtree_node * first_child;
  struct gtree_node * next_subling;
  int32_t value;
} gtree_node_t;


typedef struct
{
    gtree_node_t * root;
    int32_t value;
} gtree_t;

gtree_t * gtree_create(int32_t root_value);
gtree_node_t * gtree_create_node(gtree_t * gtree,gtree_node_t * parent_node,int32_t value);
void gtree_remove_sub(gtree_t * gtree,gtree_node_t * gtree_node);
#endif