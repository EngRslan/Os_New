#ifndef VFS_H
#define VFS_H
#include <kernel/types.h>
#define FS_BLOCKDEVICE 0x08

struct vfs_node;

typedef uint32_t (*read_callback)(struct vfs_node *,uint32_t offset,uint32_t size,ptr_t buffer);
typedef uint32_t (*write_callback)(struct vfs_node *,uint32_t offset,uint32_t size,ptr_t buffer);

typedef struct vfs_node
{
    char name[256];
    ptr_t device;
    uint32_t size;
    uint32_t fs_type;
    uint32_t flags;
    uint32_t offset;

    read_callback read;
    write_callback write;
} vfs_node_t;

typedef struct vfs_entry
{
    string_t name;
    vfs_node_t * file;
} vfs_entry_t;


void vfs_install();
void vfs_mount(string_t mount_point,vfs_node_t * node);
vfs_node_t * file_open(const string_t file_name,uint32_t flags);
int32_t vfs_read(vfs_node_t * node,uint32_t offset,uint32_t size,char * buffer);
void print_h();
#endif