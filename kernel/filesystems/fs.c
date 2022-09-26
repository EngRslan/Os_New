#include <kernel/filesystems/fs.h>
#include <kernel/filesystems/vfs.h>
#include <kernel/bits.h>
#include <string.h>
#include <logger.h>

/*
Public Functions
*/
uint32_t FsRead(FsNode *node,uint32_t offset,uint32_t size,uint8_t *buffer){
    if(node->read){
        return node->read(node,offset,size,buffer);
    }else{
        return 0;
    }

}
uint32_t FsWrite(FsNode *node,uint32_t offset,uint32_t size,uint8_t *buffer){
    if(node->write){
        return node->write(node,offset,size,buffer);
    }else{
        return 0;
    }
}
void FsOpen(FsNode *node,uint8_t read,uint8_t write){
    if(node->open)
    {
        return node->open(node);
    } else {
        return ;
    }
}
void FsClose(FsNode *node){
    if(node->close)
    {
        return node->close(node);
    } else {
        return ;
    }
}
struct DirEntry *FsReadDir(FsNode *node,uint32_t index){
    if(node->readdir && BITREAD(node->flags,(FS_DIRECTORY-1))){
        return node->readdir(node,index);
    }else{
        return NULL;
    }
}
struct FsNode *FsFindDir(FsNode *node,char *name){
    if(node->readdir && BITREAD(node->flags,(FS_DIRECTORY-1))){
        return node->finddir(node,name);
    }else{
        return NULL;
    }
}
void FsInstall(){
    VfsInstall();
}