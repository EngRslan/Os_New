#include <kernel/filesystems/vfs.h>
#include <kernel/filesystems/fs.h>
#include <kernel/datastruct/gtree.h>
#include <kernel/mem/kheap.h>
#include <kernel/bits.h>
#include <kernel/datastruct/stack.h>
#include <stddef.h>
#include <logger.h>
#include <string.h>

#define foreach_sub(item,cwn)for (FsNode * item = cwn->child; item != NULL;item = item->next)

// gtree_t *fs_tree = NULL;
FsNode *root;
VfsFileSystem * registerdFileSystems[10];

static struct DirEntry *vfs_readdir(FsNode *node, uint32_t index)
{
    static DirEntry direntry ;
    foreach_sub(item,node){
        if(item->inode == index){
            strcpy(direntry.name,item->name);
            direntry.name[strlen(direntry.name)] = 0;
            direntry.ino = item->inode;
            return &direntry;
        }
    }

    return NULL;
}
static struct FsNode *vfs_finddir(FsNode *node, char *name)
{
    foreach_sub(item,node){
        if(strcmp(item->name,name) == 0){
            return item;
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
void VfsMount(char *cpath,FsNode *node){
    char *path = kmalloc(strlen(cpath)+1);
    strcpy(path,cpath);
    path[strlen(cpath)] = 0;


    if(path[0] != '/'){
        //Currently we don't support Relative Path
        return;    
    }

    // gtree_node_t *token_node = fs_tree->root;

    FsNode *cwn = root;
    char *filepath = path+1;
    char *token = NULL;

    while ((token = strsep(&filepath,"/")))
    {
        if(filepath == NULL || strcmp(filepath,"") == 0){
            // gtree_create_child(token_node,(uint32_t)node);
            vfsLinkChild(cwn,node);
            break;
        }

        cwn = vfs_finddir(cwn,token);
        // // TODO FIX
        // //token_node = ((FsNode *)token_node)->finddir((FsNode *)token_node->value,token);
        // foreach_t(item,token_node){
        //     FsNode * snode = (FsNode *)item->value;
        //     if(strcmp(snode->name,token) == 0){
        //         token_node = item;
        //         break;
        //     }

        //     if(!item->next_subling){
        //         token_node = NULL;
        //     }
        // }

        if(!cwn){
            log_warning("Vfs Mount didn't complete directory didn't found");
            break;
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
    // gtree_node_t *token_node = fs_tree->root;
    FsNode *cwn = root;
    while ((token = strsep(&filepath,"/")))
    {
        if(token == NULL || strcmp(token,"") == 0){
            kfree(path);
            return cwn;
        }

        cwn = vfs_finddir(cwn,token);

        if(!cwn){
            break;
        }
    }

    kfree(path);
    return cwn;

}
void vfsLinkChild(FsNode *parent, FsNode *child){
    if(parent == NULL || child == NULL){
        return;
    }

    child->parent = parent;
    child->next = parent->child;
    parent->child = child;
}
void VfsInstall(){
    uint32_t inode = 0;

    root = (FsNode *)kmalloc(sizeof(FsNode));
    memset(root,0,sizeof(root));
    strcpy(root->name,"/");
    root->flags |= FS_DIRECTORY;
    root->finddir = vfs_finddir;
    root->readdir = vfs_readdir;
    
    // fs_tree = gtree_create((uint32_t)fs_root);

    FsNode * fs_dev = (FsNode *)kmalloc(sizeof(FsNode));
    memset(fs_dev,0,sizeof(FsNode));
    strcpy(fs_dev->name,"dev");
    fs_dev->flags |= FS_DIRECTORY;
    fs_dev->finddir = vfs_finddir;
    fs_dev->readdir = vfs_readdir;
    fs_dev->inode = inode++;
    vfsLinkChild(root,fs_dev);
    // gtree_create_child(fs_tree->root,(uint32_t)fs_dev);

    FsNode * fs_mnt = (FsNode *)kmalloc(sizeof(FsNode));
    memset(fs_mnt,0,sizeof(FsNode));
    strcpy(fs_mnt->name,"mnt");
    fs_mnt->flags |= FS_DIRECTORY;
    fs_mnt->finddir = vfs_finddir;
    fs_mnt->readdir = vfs_readdir;
    fs_mnt->inode = inode++;
    vfsLinkChild(root,fs_mnt);
    // gtree_create_child(fs_tree->root,(uint32_t)fs_mnt);

    FsNode * fs_tmp = (FsNode *)kmalloc(sizeof(FsNode));
    memset(fs_tmp,0,sizeof(FsNode));
    strcpy(fs_tmp->name,"tmp");
    fs_tmp->flags |= FS_DIRECTORY;
    fs_tmp->finddir = vfs_finddir;
    fs_tmp->readdir = vfs_readdir;
    fs_tmp->inode = inode++;
    vfsLinkChild(root,fs_tmp);
    // gtree_create_child(fs_tree->root,(uint32_t)fs_tmp);
}

char tabs[50];
char *print_flags(FsNode *node){
    switch (node->flags)
    {
    case FS_FILE:
        return "f";
    case FS_DIRECTORY | FS_MOUNTPOINT:
        return "md";
    case FS_DIRECTORY:
        return "d";
    case FS_BLOCKDEVICE:
        return "db";
    case FS_CHARDEVICE:
        return "dc";
    default:
        return "Un";
    }
}
void print_h(){
    // gtree_descendant_exec(fs_tree->root,print_hierarchy,0);
    FsNode *node = root;
    int depth = 0;
    log_trace("|--> %s", node->name);

    while (node)
    {
        if(node->child)
        {
            node = node->child;
            depth ++;
        }
        else if(node->next)
        {
            node = node->next;
        }
        else
        {
            // while node has parent and no parent dosn't has subling
            while (node->parent && !node->parent->next)
            {
                node = node->parent;
                depth--;
            }

            if(!node->parent){
                return;
            }
            
            node = node->parent->next;
            depth --;
        }
        
        if(node){
            for (int i = 0; i < depth; i++)
            {
                tabs[(i*4)] =':';
                tabs[(i*4)+1] = ' ';
                tabs[(i*4)+2] = ' ';
                tabs[(i*4)+3] = ' ';
                tabs[(i*4)+4] = 0x0;
            }
            log_trace("%s|--> %s(-%s)",tabs, node->name,print_flags(node));
        }
        
    }

}