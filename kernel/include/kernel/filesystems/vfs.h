#ifndef VFS_H
#define VFS_H
#include <kernel/types.h>
#include <kernel/filesystems/fs.h>

void VfsInstall();
void VfsMount(char *path,FsNode *node);
#endif