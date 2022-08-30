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
} gtree_t;

gtree_t * gtree_create(int32_t root_value);
gtree_node_t * gtree_create_child(gtree_node_t * parent_node,int32_t value);
void gtree_remove_descendant(gtree_t * gtree,gtree_node_t * gtree_node);
gtree_node_t * gtree_descendant_query(gtree_node_t * gtree_node, int(*func)(gtree_node_t *));
void gtree_descendant_exec(gtree_node_t * gtree_node, void(*func)(gtree_node_t *,int depth),int32_t is_reverse);
// void gtree_hierachy_execute(gtree_node_t * gtree_node, void(*func)(gtree_node_t *,int depth));
#endif