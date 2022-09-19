#ifndef VFS_H
#define VFS_H
#include <kernel/types.h>
#include <kernel/filesystems/fs.h>

typedef struct VfsFileSystem
{
    void (*mount)(char *path,char *mountpoint);
    char name[50];
} VfsFileSystem;

void VfsRegisterFileSystem(VfsFileSystem *filesystem);
void VfsInstall();
void VfsMount(char *path,FsNode *node);
void VfsMountFs(char *path,char *mountpoint,char *fsname);
FsNode *VfsGetMountpoint(char *path);

#endif