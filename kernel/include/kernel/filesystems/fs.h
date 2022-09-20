#ifndef FS_H
#define FS_H
#include <kernel/types.h>

typedef enum{
    FS_FILE         = 0x01,
    FS_DIRECTORY    = 0x02,
    FS_CHARDEVICE   = 0x03,
    FS_BLOCKDEVICE  = 0x04,
    FS_PIPE         = 0x05,
    FS_SYMLINK      = 0x06,
    FS_MOUNTPOINT   = 0x08
} FsNodeType;

typedef struct DirEntry
{
    char name[128];
    uint32_t ino;
} DirEntry;


struct FsNode;
typedef uint32_t (*ReadCallback)(struct FsNode *node,uint32_t offset,uint32_t size,uint8_t *buffer);
typedef uint32_t (*WriteCallback)(struct FsNode *node,uint32_t offset,uint32_t size,uint8_t *buffer);
typedef void (*OpenCallback)(struct FsNode *node);
typedef void (*CloseCallback)(struct FsNode *node);
typedef struct DirEntry *(*ReadDirCallback)(struct FsNode *node,uint32_t);
typedef struct FsNode *(*FindDirCallback)(struct FsNode *node,char *name);


typedef struct FsNode{
    char name[128];
    uint32_t mask;
    uint32_t uid;
    uint32_t gid;
    uint32_t flags;
    uint32_t inode;
    uint32_t length;
    uint32_t impl;
    uint8_t *buffer;

    ReadCallback read;
    WriteCallback write;
    OpenCallback open;
    CloseCallback close;
    ReadDirCallback readdir;
    FindDirCallback finddir;
    struct FsNode *ptr;
}FsNode;

extern FsNode *fs_root;

void FsInstall();
uint32_t FsRead(FsNode *node,uint32_t offset,uint32_t size,uint8_t *buffer);
uint32_t FsWrite(FsNode *node,uint32_t offset,uint32_t size,uint8_t *buffer);
void FsOpen(FsNode *node,uint8_t read,uint8_t write);
void FsClose(FsNode *node);
struct DirEntry *FsReadDir(FsNode *node,uint32_t index);
FsNode *FsFindDir(FsNode *node,char *name);

#endif