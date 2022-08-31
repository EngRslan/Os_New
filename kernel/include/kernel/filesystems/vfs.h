#ifndef VFS_H
#define VFS_H
#include <kernel/types.h>

typedef struct
{

} vfs_node_t;

typedef struct
{
    string_t name;
    vfs_node_t * file;
} vfs_entry_t;


void vfs_install();
#endif