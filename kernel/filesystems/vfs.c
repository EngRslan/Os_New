#include <kernel/filesystems/vfs.h>
#include <kernel/filesystems/fs.h>
#include <kernel/datastruct/gtree.h>
#include <kernel/mem/kheap.h>
#include <stddef.h>
#include <logger.h>
#include <string.h>

gtree_t *fs_tree = NULL;

VfsFileSystem * registerdFileSystems[10];

static struct DirEntry *vfs_readdir(FsNode *node, uint32_t index)
{
    static DirEntry direntry ;
    foreach_t(item,fs_tree->root){
        FsNode *fs_node = (FsNode *)item->value;
        if(fs_node->inode == index){
            strcpy(direntry.name,fs_node->name);
            direntry.name[strlen(direntry.name)] = 0;
            direntry.ino = fs_node->inode;
            return &direntry;
        }
    }

    return NULL;
}
static struct FsNode *vfs_finddir(FsNode *node, char *name)
{
    foreach_t(item,fs_tree->root){
        FsNode *fs_node = (FsNode *)item->value;
        if(strcmp(fs_node->name,name) == 0){
            return fs_node;
        }
    }

    return NULL;
}
void VfsRegisterFileSystem(VfsFileSystem *filesystem){
    if(!filesystem || !filesystem->mount || filesystem->name[0] == 0x0)
    {
        return;
    }

    for (uint8_t i = 0; i < 10; i++)
    {
        if(registerdFileSystems[i]){
            continue;
        }

        registerdFileSystems[i] = filesystem;
        break;
    }
}
void VfsMountFs(char *path,char *mountpoint,char *fsname){
    VfsFileSystem * fs = NULL;
    for (uint8_t i = 0; i < 10; i++)
    {
        if(registerdFileSystems[i] && strcmp(registerdFileSystems[i]->name,fsname)==0){
            fs = registerdFileSystems[i];
            break;
        }
    }

    if(fs){
        fs->mount(path,mountpoint);
    }
}
void VfsMountChild(FsNode *parent,FsNode *child){
    
}
void VfsMount(char *cpath,FsNode *node){
    char *path = kmalloc(strlen(cpath)+1);
    strcpy(path,cpath);
    path[strlen(cpath)] = 0;


    if(path[0] != '/'){
        //Currently we don't support Relative Path
        return;    
    }

    gtree_node_t *token_node = fs_tree->root;

    char *filepath = path+1;
    char *token = NULL;

    while ((token = strsep(&filepath,"/")))
    {
        if(filepath == NULL || strcmp(filepath,"") == 0){
            gtree_create_child(token_node,(uint32_t)node);
            kfree(path);
            return;
        }

        // TODO FIX
        //token_node = ((FsNode *)token_node)->finddir((FsNode *)token_node->value,token);
        foreach_t(item,token_node){
            FsNode * snode = (FsNode *)item->value;
            if(strcmp(snode->name,token) == 0){
                token_node = item;
                break;
            }

            if(!item->next_subling){
                token_node = NULL;
            }
        }

        if(!token_node){
            log_debug("Error parent directory not mounted");
            kfree(path);
            return;
        }
    }

    kfree(path);

}
FsNode *VfsGetMountpoint(char *cpath){
    char *path = kmalloc(strlen(cpath)+1);
    strcpy(path,cpath);
    path[strlen(cpath)] = 0;

    if(path[0] != '/'){
        //Currently we don't support Relative Path
        return NULL;    
    }

    char *filepath = path+1;
    char *token = NULL;
    gtree_node_t *token_node = fs_tree->root;

    while ((token = strsep(&filepath,"/")))
    {
        if(token == NULL || strcmp(token,"") == 0){
            kfree(path);
            return (FsNode *)token_node->value;
        }

        foreach_t(item,token_node){
            FsNode * snode = (FsNode *)item->value;
            if(strcmp(snode->name,token) == 0){
                token_node = item;
                break;
            }

            if(!item->next_subling){
                token_node = NULL;
            }
        }

        if(!token_node){
            log_debug("Error parent directory not mounted");
            kfree(path);
            return NULL;
        }
    }

    kfree(path);
    return token_node?(FsNode *)token_node->value:NULL;

}
void VfsInstall(){
    uint32_t inode = 0;

    FsNode * fs_root = (FsNode *)kmalloc(sizeof(FsNode));
    memset(fs_root,0,sizeof(fs_root));
    strcpy(fs_root->name,"/");
    fs_root->flags |= FS_DIRECTORY;
    fs_root->finddir = vfs_finddir;
    fs_root->readdir = vfs_readdir;
    fs_tree = gtree_create((uint32_t)fs_root);

    FsNode * fs_dev = (FsNode *)kmalloc(sizeof(FsNode));
    memset(fs_dev,0,sizeof(FsNode));
    strcpy(fs_dev->name,"dev");
    fs_dev->flags |= FS_DIRECTORY;
    fs_dev->finddir = vfs_finddir;
    fs_dev->readdir = vfs_readdir;
    fs_dev->inode = inode++;
    gtree_create_child(fs_tree->root,(uint32_t)fs_dev);

    FsNode * fs_mnt = (FsNode *)kmalloc(sizeof(FsNode));
    memset(fs_mnt,0,sizeof(FsNode));
    strcpy(fs_mnt->name,"mnt");
    fs_mnt->flags |= FS_DIRECTORY;
    fs_mnt->finddir = vfs_finddir;
    fs_mnt->readdir = vfs_readdir;
    fs_mnt->inode = inode++;
    gtree_create_child(fs_tree->root,(uint32_t)fs_mnt);

    FsNode * fs_tmp = (FsNode *)kmalloc(sizeof(FsNode));
    memset(fs_tmp,0,sizeof(FsNode));
    strcpy(fs_tmp->name,"tmp");
    fs_tmp->flags |= FS_DIRECTORY;
    fs_tmp->finddir = vfs_finddir;
    fs_tmp->readdir = vfs_readdir;
    fs_tmp->inode = inode++;
    gtree_create_child(fs_tree->root,(uint32_t)fs_tmp);
}

char tabs[50];
void print_hierarchy(gtree_node_t * node,int depth){
  if(depth == 0){
    tabs[0] = 0;
  }
  for (int i = 0; i < depth; i++)
  {
    tabs[i]='\t';
    tabs[i+1]=0x0;
  }
  FsNode *entry = (FsNode *)node->value;
  
  log_trace("%s--| %s",tabs, entry->name);
}
void print_h(){
    gtree_descendant_exec(fs_tree->root,print_hierarchy,0);
}