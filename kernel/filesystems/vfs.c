#include <kernel/filesystems/vfs.h>
#include <kernel/datastruct/tree.h>
#include <kernel/mem/kheap.h>
#include <string.h>

tree_t * vfs_tree;

char *strdup(const char *src){
    char *dst = kmalloc(strlen(src)+1);
    strcpy(dst,src);
    return dst;
}
void vfs_install(){
    vfs_tree = tree_create();
    vfs_entry_t * root = (vfs_entry_t *)kmalloc(sizeof(vfs_entry_t));
    root->name = strdup("root");
    root->file = NULL;
}