#include <kernel/filesystems/vfs.h>
#include <kernel/datastruct/gtree.h>
#include <kernel/mem/kheap.h>
#include <string.h>
#include <logger.h>

gtree_t * vfs_tree;
vfs_node_t * vfs_root;

char *strdup(const char *src){
    char *dst = kmalloc(strlen(src)+1);
    strcpy(dst,src);
    return dst;
}
vfs_entry_t * get_mountpoint_recur(string_t path,gtree_node_t * subroot){
    string_t token = strsep(&path,"/");
    if(token == NULL || strcmp(token,"") == 0){
        struct vfs_entry * entry = (struct vfs_entry *)subroot->value;
        return entry->file;
    }

    gtree_node_t * child = subroot->first_child;
    int32_t found = 0;

    while (child)
    {
        struct vfs_entry * entry = (struct vfs_entry *)(child->value);
        if(strcmp(entry->name,token)==0){
            found = 1;
            subroot = child;
            break;
        }

        child = child->next_subling;
    }

    if(!found){
        return NULL;
    }

    return get_mountpoint_recur(path,subroot);

}

vfs_node_t * get_mountpoint(string_t path){
    if(strlen(path) > 1 && path[strlen(path) - 1] == '/'){
        path[strlen(path) - 1] = 0;
    }
    if(!path || path[0] != '/')return NULL;
    if(strlen(path) == 1){
        struct vfs_entry * entry = (struct vfs_entry *)vfs_tree->root->value;
        entry->file;
    }
    path++;
    return get_mountpoint_recur(path,vfs_tree->root);
}
void vfs_mount_recur(string_t path,gtree_node_t * subroot,vfs_node_t * fs_obj){
    
    string_t token = strsep(&path,"/");
    if(token == NULL || strcmp(token,"") == 0){
        struct vfs_entry * entry = (struct vfs_entry *)subroot->value;
        if(entry->file){
            log_warning("path already mounted");
            return;
        }
        if(strcmp(entry->name,"/")==0)vfs_root = fs_obj;
        entry->file = fs_obj;
        return;
    }

    gtree_node_t * child = subroot->first_child;
    int32_t found = 0;

    while (child)
    {
        struct vfs_entry * entry = (struct vfs_entry *)(child->value);
        if(strcmp(entry->name,token)==0){
            found = 1;
            subroot = child;
            break;
        }

        child = child->next_subling;
    }


    if(!found){
        struct vfs_entry * entry = kmalloc(sizeof(struct vfs_entry));
        entry->name = strdup(token);
        subroot = gtree_create_child(subroot,(uint32_t)entry);
    }
    
    vfs_mount_recur(path,subroot,fs_obj);
    
}
void vfs_mount(string_t path,vfs_node_t * node){
    node->fs_type = 0;  
    if(path[0] == '/' && strlen(path) == 1){
        struct vfs_entry * entry = (struct vfs_entry *)vfs_tree->root->value;
        if(entry->file){
            log_warning("The path already mounted");
        }
        vfs_root = node;
        entry->file = node;
        return;
    }

    vfs_mount_recur(path+1,vfs_tree->root,node);
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
  struct vfs_entry * entry = (struct vfs_entry *)node->value;
  
  log_trace("%s--| %s",tabs, entry->name);
}
void print_h(){
    gtree_descendant_exec(vfs_tree->root,print_hierarchy,0);
}
void vfs_install(){
    vfs_entry_t * root = (vfs_entry_t *)kmalloc(sizeof(vfs_entry_t));
    root->name = strdup("root");
    root->file = NULL;
    vfs_tree = gtree_create((uint32_t)root);
}