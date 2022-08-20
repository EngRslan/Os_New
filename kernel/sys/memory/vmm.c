#include <kernel/mem/pmm.h>
#include <kernel/mem/vmm.h>
#include <stddef.h>
#include <string.h>
extern unsigned int _lower_kernel_end;
extern unsigned int _higher_kernel_start;
extern unsigned int _kernel_end;
void map_page(page_directory_t * dir,v_addr_t virtual_address,p_frame_t physical_frame,unsigned int is_user,unsigned int is_writable){
    page_table_t * pages_table = NULL;
    if(!dir){
        return ;
    }

    unsigned int table_index = DIR_INDEX(virtual_address);
    unsigned int page_index = PAGE_INDEX(virtual_address);

    page_table_directory_t * page_dir_entry = &dir->tables[table_index];

    if(!page_dir_entry->present){
        pages_table = (page_table_t *) allocate_block();
        memset(pages_table,0,sizeof(page_table_t));
        page_dir_entry->present = 1;
        page_dir_entry->rw = is_writable;
        page_dir_entry->user = is_user;
        page_dir_entry->frame = (unsigned int)pages_table >> 12;
    }else{
        pages_table = (page_table_t *)((unsigned int)page_dir_entry->frame << 12);
    }

    page_table_entry_t * page_entry = &pages_table->pages[page_index];
    page_entry->present = 1;
    page_entry->rw = is_writable;
    page_entry->user = is_user;
    page_entry->frame = physical_frame >> 12;
}

void map_region(page_directory_t * dir, v_addr_t start,p_frame_t physical_frame,unsigned int total_pages,unsigned int is_user,unsigned int is_writable){
    start = start & 0xFFFFF000;

    for (unsigned int i = 0; i < total_pages; i++)
    {
        map_page(dir,start,physical_frame,is_user,is_writable);
        start += PAGE_SIZE;
        physical_frame += BLOCK_SIZE;
    }
    
}
void switch_directory(page_directory_t * dir){
    unsigned int d = (unsigned int)dir;
    __asm__ __volatile__("mov %0,%%CR3"::"r"(d));
}
void vmm_install(){
    page_directory_t * default_dir = (page_directory_t *)allocate_block();
    //Map defualt directory
    memset(default_dir,0,sizeof(page_directory_t));
    //map_page(default_dir,(v_addr_t)default_dir,(p_frame_t)default_dir,0,1);
    //Map First 4MB
    map_region(default_dir,0,0,0x400000/PAGE_SIZE,0,1);
    
    //MAP Kernel
    // unsigned int kernel_pages = (_kernel_end - _higher_kernel_start)/PAGE_SIZE;
    // if((_kernel_end - _higher_kernel_start)%PAGE_SIZE)kernel_pages++;
    map_region(default_dir,(v_addr_t)&_higher_kernel_start,0,0x400000/PAGE_SIZE,0,1);

    //MAP PMM Bitmap Area
    // unsigned int pmm_size = bitmap_size/PAGE_SIZE;
    // if(pmm_size%PAGE_SIZE)pmm_size++;
    // map_region(default_dir,(v_addr_t)_higher_kernel_start,(p_frame_t)bitmap_start,pmm_size,0,1);

    switch_directory(default_dir);

}

