#ifndef LINK_H
#define LINK_H
#include <stddef.h>

typedef struct Link
{
    struct Link *next;
    struct Link *prev;
} Link;

static inline void LinkBefore(Link *a, Link *x)
{
    Link *p = a == NULL?NULL:a->prev;
    Link *n = a;
    n->prev = x;
    x->next = n;
    x->prev = p;
    if(p){
        p->next = x;
    }
}
#endif