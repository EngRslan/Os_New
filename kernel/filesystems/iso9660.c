#include <kernel/filesystems/iso9660.h>
#include <kernel/filesystems/vfs.h>
#include <kernel/mem/kheap.h>
#include <kernel/bits.h>
#include <string.h>
#include <logger.h>
#include <stdbool.h>
#define foreach_entry(entry,buffer)for(Iso9660Directory *entry = (Iso9660Directory *)buffer;entry->length > 0;entry=(Iso9660Directory *)((uint8_t *)entry + entry->length))
typedef struct Iso9660Susp
{
    char signature[2];
    uint8_t length;
    uint8_t version;
}Iso9660Susp;

typedef union Iso9660RripNm
{
    uint8_t flagbits;
    struct 
    {
        uint8_t isContinue :1;
        uint8_t isCurrent :1;
        uint8_t isParent :1;
        uint8_t reserved :5;
    };
    
}Iso9660RripNm;

void mount(char *device, char* mount_point);
void Open(FsNode *node);
uint32_t Read(FsNode *node,uint32_t offset,uint32_t size,uint8_t *buffer);
uint32_t Write(FsNode *node,uint32_t offset,uint32_t size,uint8_t *buffer);
void Open(FsNode *node);
void Close(FsNode *node);
DirEntry *ReadDir(FsNode *node,uint32_t index);
FsNode *FindDir(FsNode *node,char *name);

typedef struct Iso9660DirectoryDateTime
{
    uint8_t years;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t offset;
} __attribute__((packed)) Iso9660DirectoryDateTime ;
typedef struct Iso9660Directory
{
    uint8_t     length                        ;
    uint8_t     attribute                     ;
    uint32_t    location_lba_lsb             ;
    uint32_t    location_lba_msb             ;
    uint32_t    size_lsb                     ;
    uint32_t    size_msb                     ;
    Iso9660DirectoryDateTime creation_date     ;  
    union
    {
        uint8_t flagsbits;
        struct
        {
            uint8_t hidden          :1;
            uint8_t directory       :1;
            uint8_t associatedFile  :1;
            uint8_t fileFormatExt   :1;
            uint8_t permissionExt   :1;
            uint8_t _               :2;
            uint8_t notFinalDir     :1;
        };
    };
    uint8_t     interleaved_unit_size         ;
    uint8_t     interleave_gap_size           ;
    uint16_t    volume_sequence_number_lsb   ;
    uint16_t    volume_sequence_number_msb   ;
    uint8_t     length_file_identifier        ;
    uint8_t     file_identifier[]               ;
} __attribute__((packed)) Iso9660Directory;
typedef struct {
    uint8_t     type_code                              ;
    uint8_t     standard_identifier[5]                 ;
    uint8_t     version                                ;
    uint8_t     unused_1                               ;
    uint8_t     system_identifier[32]                  ;
    uint8_t     volume_identifier[32]                  ;
    uint8_t     unused_2[8]                            ;
    uint32_t    volume_space_size_lsb                 ;
    uint32_t    volume_space_size_msb                 ;
    uint8_t     unused_3[32]                           ;
    uint16_t    volume_set_size_lsb                   ;
    uint16_t    volume_set_size_msb                   ;
    uint16_t    volume_sequence_number_lsb            ;
    uint16_t    volume_sequence_number_msb            ;
    uint16_t    logical_block_size_lsb                ;
    uint16_t    logical_block_size_msb                ;
    uint32_t    path_table_size_lsb                   ;
    uint32_t    path_table_size_msb                   ;
    uint32_t    path_table_location_lsb               ;
    uint32_t    optional_path_table_location_lsb      ;
    uint32_t    path_table_location_msb               ;
    uint32_t    optional_path_table_location_msb      ;
    struct Iso9660Directory root_directory                   ;
} __attribute__((packed)) PVD_t;
typedef struct Iso9660
{
    uint32_t sector_size;
    PVD_t * pvd;
    FsNode * device;
} Iso9660;

void normalizeFilename(char *name,char *file_identifier,uint8_t file_identifier_length){
    if (file_identifier[0] == 0x0)
    {
        strcpy(name,".");
        
    }else if(file_identifier[0] == 0x01){
        strcpy(name,"..");
    }else{
        // memcpy(direntry.name,dir->file_identifier,dir->length_file_identifier);
        char *s = file_identifier;
        uint32_t i = 0;
        while (*s && i <= file_identifier_length && *s != ';')
        {
            name[i] = *s;
            i++;
            s++;
        }
        
        name[i]=0x0;
    }
}
void read_disk_block(Iso9660 * fs,uint32_t block,char * buffer){
    FsRead(fs->device,fs->sector_size * block,fs->sector_size,(uint8_t *)buffer);
}
void iso9660_install(){
    VfsFileSystem *fs = (VfsFileSystem *)kmalloc(sizeof(VfsFileSystem));
    strcpy(fs->name,ISO9660_FILESYSTEM_NAME);
    fs->mount = mount;
    VfsRegisterFileSystem(fs);
}
uint32_t Read(FsNode *node,uint32_t offset,uint32_t size,uint8_t *buffer){
    if(node->flags != FS_FILE){
        return 1;
    }

    if(offset + size > node->length){
        return 2;
    }

    memcpy(buffer,node->buffer+offset,size);
    return 0;
}
uint32_t Write(FsNode *node,uint32_t offset,uint32_t size,uint8_t *buffer){
    return 0;
}

void mountDirectory(FsNode *parent)
{
    Iso9660 *fs = (Iso9660 *)parent->impl;
    int totalsectors = parent->length/fs->sector_size;
    int cwsectors = 1;
    // for(
    //     Iso9660Directory *entry = (Iso9660Directory *)parent->buffer;
    //     true;
    //     entry=(Iso9660Directory *)((uint8_t *)entry + entry->length))
    // foreach_entry(entry,parent->buffer)
    Iso9660Directory *entry = (Iso9660Directory *)parent->buffer;
    while (entry)
    {
        // char filename[128];
        // normalizeFilename(filename,entry->file_identifier,entry->length_file_identifier);
        // log_debug("mounter dir %s",filename);
        
        if(entry->file_identifier[0]=='\0' || entry->file_identifier[0]=='\1'){
            entry=(Iso9660Directory *)((uint8_t *)entry + entry->length);
            continue;
        }
        FsNode *node = (FsNode *)kmalloc(sizeof(FsNode));
        Iso9660Susp *susp = (Iso9660Susp *)(entry->file_identifier + entry->length_file_identifier+(entry->length_file_identifier % 2 == 0?1:0));
        bool isFound=false;
        while ((void *)susp+1 < (void *)entry+entry->length)
        {

            if(susp->signature[0] != 'N' && susp->signature[1] != 'M'){
                susp = (Iso9660Susp *)((void *)susp + susp->length);
                continue;
            }

            Iso9660RripNm *nmflags  = (Iso9660RripNm *)(susp+1);
            //TODO check continu flag
            memcpy(node->name,(void *)susp + 5,susp->length - 5);
            isFound = true;
            break;

        }
        
        if(!isFound){
            normalizeFilename(node->name,(char *)entry->file_identifier,entry->length_file_identifier);
        }
        node->length = entry->size_lsb;
        node->inode = entry->location_lba_lsb;
        node->impl = parent->impl;
        node->buffer = NULL;
        node->flags |= entry->directory?FS_DIRECTORY:FS_FILE;
        node->open = Open;
        node->close = Close;
        node->read = Read;
        node->write = Write;
        node->finddir = FindDir;
        node->readdir = ReadDir;
        vfsLinkChild(parent,node);

        entry=(Iso9660Directory *)((uint8_t *)entry + entry->length);
        if(entry->length == 0 && cwsectors < totalsectors)
        {
            cwsectors ++;
            entry=(Iso9660Directory *)(parent->buffer + ((cwsectors-1) * fs->sector_size));
            continue;
        }
        else if(entry->length > 0)
        {
            continue;
        }
        else
        {
            break;
        }
    }
    
    

}
void Open(FsNode *node){
    if(node->buffer){
        return;
    }
    Iso9660 *fs = (Iso9660 *)node->impl;
    uint8_t *buffer = (uint8_t *)kmalloc(node->length);
    fs->device->read(fs->device,node->inode * fs->sector_size,node->length,buffer);
    node->buffer = buffer;
    
    if(BITREAD(node->flags,FS_DIRECTORY-1)){
        mountDirectory(node);
    }
}
void Close(FsNode *node){
    if(!node->buffer){
        return;
    }

    kfree(node->buffer);
    node->buffer = NULL;
}
DirEntry *ReadDir(FsNode *node,uint32_t index){
    if(!node->buffer){
        return NULL;
    }
    Iso9660Directory *dir = (Iso9660Directory *)node->buffer;
    if(dir->length <= 0)
    {
        return NULL;
    }
    uint32_t i = 0;
    while (i < index)
    {
        dir = (Iso9660Directory *)((uint8_t *)dir + dir->length);
        i++;
    }

    if(dir->length <= 0)
    {
        return NULL;
    }

    static DirEntry direntry;
    direntry.ino = index;
    normalizeFilename(direntry.name,(char *)dir->file_identifier,dir->length_file_identifier);
    return &direntry;
}
//TODO Don't forget to re write this function
FsNode *FindDir(FsNode *node,char *name){
    if(!node->buffer){
        return NULL;
    }
    Iso9660Directory *dir = (Iso9660Directory *)node->buffer;

    if(dir->length <= 0)
    {
        return NULL;
    }
    
    char _normalizedFilename[128];
    normalizeFilename(_normalizedFilename,(char *)dir->file_identifier,dir->length_file_identifier);

    while (strcmp(name,_normalizedFilename) != 0)
    {
        dir = (Iso9660Directory *)((uint8_t *)dir + dir->length);
        normalizeFilename(_normalizedFilename,(char *)dir->file_identifier,dir->length_file_identifier);
    }

    if(dir->length <= 0)
    {
        return NULL;
    }
 return NULL;

}
void mount(char *device, char* mount_point){
    Iso9660 * isofs = (Iso9660 *)kmalloc(sizeof(Iso9660));
    isofs->device = VfsGetMountpoint(device);
    isofs->sector_size = 2048;
    isofs->pvd = (PVD_t *)kmalloc(isofs->sector_size);
    read_disk_block(isofs,0x10,(void *)isofs->pvd);
    isofs->sector_size = isofs->pvd->logical_block_size_lsb;
    FsNode * node = kmalloc(sizeof(FsNode));
    strcpy(node->name,"cdrom");
    node->impl = (uint32_t)isofs;
    node->inode = (uint32_t)isofs->pvd->root_directory.location_lba_lsb;
    node->length = (uint32_t)isofs->pvd->root_directory.size_lsb;
    node->flags |= FS_DIRECTORY;
    node->flags |= FS_MOUNTPOINT;
    node->open = Open;
    node->close = Close;
    node->read = Read;
    node->write = Write;
    node->finddir = FindDir;
    node->readdir = ReadDir;

    VfsMount(mount_point,node);
    Open(node);

}