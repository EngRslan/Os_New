#ifndef VMEM_H
#define VMEM_H
#define PAGE_SIZE 0x1000
#define DIR_INDEX(vaddr)((v_addr_t)vaddr >> 22)
#define PAGE_INDEX(vaddr)(((v_addr_t)vaddr >> 12)&0x3FF)
#define PHYSICAL_ADDRESS(entry)(* entry & ~0xFFF)

typedef unsigned int p_frame_t;
typedef unsigned int v_addr_t;


typedef struct page_table_entry{
    unsigned int present    :1;
    unsigned int rw         :1;
    unsigned int user       :1;
    unsigned int reserved   :2;
    unsigned int accessed   :1;
    unsigned int dirty      :1;
    unsigned int reserved2  :2;
    unsigned int available  :3;
    unsigned int frame      :20;
} page_table_entry_t;

typedef struct page_directory_entry{
    unsigned int present    : 1;
    unsigned int rw         : 1;
    unsigned int user       : 1;
    unsigned int w_through  : 1;
    unsigned int cache      : 1;
    unsigned int access     : 1;
    unsigned int reserved   : 1;
    unsigned int page_size  : 1;
    unsigned int global     : 1;
    unsigned int available  : 3;
    unsigned int frame      : 20;
} page_table_directory_t;

typedef struct page_table
{
    page_table_entry_t pages[1024];
} page_table_t;

typedef struct page_directory
{
    page_table_directory_t tables[1024];
} page_directory_t;

extern page_directory_t * kernel_directory;

void vmm_install();
void allocate_page(page_directory_t * dir,v_addr_t virtual_address,unsigned int is_user,unsigned int is_writable);
void allocate_region(page_directory_t * dir,v_addr_t virtual_address,unsigned int total_pages,unsigned int is_user,unsigned int is_writable);
void free_page(page_directory_t * dir,v_addr_t virtual_address);
#endif