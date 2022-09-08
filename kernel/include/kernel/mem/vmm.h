#ifndef VMEM_H
#define VMEM_H
#include "../types.h"
#include <kernel/sys/kernel_address.h>
#define PAGE_SIZE 0x1000
#define DIR_INDEX(vaddr)((v_addr_t)vaddr >> 22)
#define PAGE_INDEX(vaddr)(((v_addr_t)vaddr >> 12)&0x3FF)
#define GET_VIRTUAL_ADDRESS(pde,pte)((pde << 22)|(pte << 12)|0)
#define GET_VIRTUAL_TABLE_ADDRESS(pdi)(GET_VIRTUAL_ADDRESS(KERNEL_TABLES_MAP_DIR_INDX,pdi))

typedef uint32_t p_frame_t;
typedef uint32_t v_addr_t;


typedef struct page_table_entry{
    uint32_t present    :1;
    uint32_t rw         :1;
    uint32_t user       :1;
    uint32_t reserved   :2;
    uint32_t accessed   :1;
    uint32_t dirty      :1;
    uint32_t reserved2  :2;
    uint32_t available  :3;
    uint32_t frame      :20;
} page_table_entry_t;

typedef struct page_directory_entry{
    uint32_t present    : 1;
    uint32_t rw         : 1;
    uint32_t user       : 1;
    uint32_t w_through  : 1;
    uint32_t cache      : 1;
    uint32_t access     : 1;
    uint32_t reserved   : 1;
    uint32_t page_size  : 1;
    uint32_t global     : 1;
    uint32_t available  : 3;
    uint32_t frame      : 20;
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
void allocate_page(page_directory_t * dir,v_addr_t virtual_address,uint32_t is_user,uint32_t is_writable);
void kallocate_page(v_addr_t virtual_address);
void allocate_region(page_directory_t * dir,v_addr_t virtual_address,uint32_t total_pages,uint32_t is_user,uint32_t is_writable);
void callocate_region(page_directory_t * dir,v_addr_t virtual_address,uint32_t total_pages,uint32_t is_user,uint32_t is_writable);
void free_page(page_directory_t * dir,v_addr_t virtual_address);
uint32_t virtual2physical(v_addr_t addr);
#endif