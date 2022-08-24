#include <kernel/types.h>
#include <kernel/sys/kernel_address.h>
#include <kernel/mem/pmm.h>
#include <kernel/mem/vmm.h>
#include <stddef.h>
#include <string.h>

page_directory_t * kernel_directory;
page_table_t * kernel_tables_map;
int is_pagging_done = 0;

void map_page(page_directory_t * dir,v_addr_t virtual_address,p_frame_t physical_frame,uint32_t is_user,uint32_t is_writable);
void map_region(page_directory_t * dir, v_addr_t start,p_frame_t physical_frame,uint32_t total_pages,uint32_t is_user,uint32_t is_writable);
void flush_tlb_entry(v_addr_t address)
{
    __asm__ __volatile__ ("cli; invlpg (%0); sti" : : "r"(address) );
}
void free_page(page_directory_t * dir,v_addr_t virtual_address){
    page_table_directory_t * dir_entry = &dir->tables[DIR_INDEX(virtual_address)];
    if(!dir_entry->present){
        return;
    }

    page_table_t * pages_table = (page_table_t *) GET_VIRTUAL_TABLE_ADDRESS(DIR_INDEX(virtual_address));
    page_table_entry_t * page_entry = &pages_table->pages[PAGE_INDEX(virtual_address)];
    if(!page_entry->present){
        return;
    }

    page_entry->present = 0;
    free_block((void *)(page_entry->frame << 12));
    flush_tlb_entry(virtual_address);
}
void allocate_page(page_directory_t * dir,v_addr_t virtual_address,uint32_t is_user,uint32_t is_writable){
    p_frame_t allocate_address = (p_frame_t) allocate_block();
    map_page(dir,virtual_address,allocate_address,is_user,is_writable);
}

void kallocate_page(v_addr_t virtual_address){
    allocate_page(kernel_directory,virtual_address,0,1);
}

void allocate_region(page_directory_t * dir,v_addr_t virtual_address,uint32_t total_pages,uint32_t is_user,uint32_t is_writable){
    
    for (uint32_t i = 0; i < total_pages; i++)
    {
        allocate_page(dir,virtual_address,is_user,is_writable);
        virtual_address += PAGE_SIZE;
    }
    
}
void map_dir_entry(page_table_directory_t * pde,p_frame_t frame,uint32_t is_user,uint32_t is_writable){
    pde->present = 1;
    pde->rw = is_writable;
    pde->user = is_user;
    pde->frame = (uint32_t)frame >> 12;
}
void map_table_entry(page_table_entry_t * pte,p_frame_t frame,uint32_t is_user,uint32_t is_writable){
    pte->present = 1;
    pte->rw = is_writable;
    pte->user = is_user;
    pte->frame = (uint32_t)frame >> 12;
}
page_table_t * allocate_virtual_table(page_directory_t * dir, uint32_t pdi,uint32_t is_user,uint32_t is_writable){
    page_table_t * pages_table = NULL;
    page_table_directory_t * page_dir_entry = &dir->tables[pdi];
    void * ph_page_table = (page_table_t *) allocate_block();
    
    page_table_entry_t * mapping_entry = &kernel_tables_map->pages[pdi];
    map_table_entry(mapping_entry,(p_frame_t)ph_page_table,is_user,is_writable);
    flush_tlb_entry((v_addr_t)kernel_tables_map);

    if(is_pagging_done){
        pages_table = (page_table_t *) GET_VIRTUAL_TABLE_ADDRESS(pdi);
    }
    else{
        pages_table = (page_table_t *) ph_page_table;
    }

    map_dir_entry(page_dir_entry,(p_frame_t)ph_page_table,is_user,is_writable);
    memset(pages_table,0,sizeof(page_table_t)); 
    return pages_table;
}
void map_page(page_directory_t * dir,v_addr_t virtual_address,p_frame_t physical_frame,uint32_t is_user,uint32_t is_writable){
    page_table_t * pages_table = NULL;
    if(!dir){
        return ;
    }

    uint32_t table_index = DIR_INDEX(virtual_address);
    uint32_t page_index = PAGE_INDEX(virtual_address);

    page_table_directory_t * page_dir_entry = &dir->tables[table_index];

    if(!page_dir_entry->present){
        pages_table = allocate_virtual_table(dir,table_index,is_user,is_writable);
    }else{
        if(is_pagging_done){
            pages_table = (page_table_t *) GET_VIRTUAL_TABLE_ADDRESS(table_index);
        }
        else{
            pages_table = (page_table_t *)((uint32_t)page_dir_entry->frame << 12);
        }
    }

    page_table_entry_t * page_entry = &pages_table->pages[page_index];
    map_table_entry(page_entry,physical_frame,is_user,is_writable);
}
void map_region(page_directory_t * dir, v_addr_t start,p_frame_t physical_frame,uint32_t total_pages,uint32_t is_user,uint32_t is_writable){
    start = start & 0xFFFFF000;

    for (uint32_t i = 0; i < total_pages; i++)
    {
        map_page(dir,start,physical_frame,is_user,is_writable);
        start += PAGE_SIZE;
        physical_frame += BLOCK_SIZE;
    }
    
}
void switch_directory(page_directory_t * dir){
    uint32_t d = (uint32_t)dir;
    __asm__ __volatile__("mov %0,%%CR3"::"r"(d));
}
void vmm_install(){
    page_directory_t * default_dir = (page_directory_t *)allocate_block();
    memset(default_dir,0,sizeof(page_directory_t));

    kernel_tables_map = (page_table_t *)allocate_block();
    memset(kernel_tables_map,0,sizeof(page_table_t));
    
    // Map page tables space MAP ALL TABLES PAGES TO USE IT AGAIN
    map_page(default_dir,(v_addr_t)GET_VIRTUAL_ADDRESS(0x300,0x3FF),(p_frame_t)kernel_tables_map,0,1);
    page_table_directory_t * dir400 = &default_dir->tables[0x301];
    dir400->present = 1,
    dir400->user = 0;
    dir400->rw = 1;
    dir400->frame = (uint32_t)kernel_tables_map >> 12;


    //Map defualt directory
    map_page(default_dir,(v_addr_t)GET_VIRTUAL_ADDRESS(0x300,0x3FE),(p_frame_t)default_dir,0,1);
    
    //MAP DEFAULT VGA ADDRESS IDENTITY
    map_region(default_dir,0xb8000,0xb8000,8,0,1);

    //map memory bitmap AS IS
    uint32_t map_size = (uint32_t)&_virtual_memory_free_address - (uint32_t)&_virtual_memory_bitmap_address;
    map_region(default_dir,(v_addr_t)&_physical_memory_bitmap_address,(p_frame_t)&_physical_memory_bitmap_address,map_size/PAGE_SIZE,0,1);
    
    //MAP Kernel
    uint32_t kernel_size = (uint32_t)&_virtual_memory_bitmap_address - (uint32_t)&_virtual_kernel_address;
    map_region(default_dir,(v_addr_t)&_virtual_kernel_address,(uint32_t)&_physical_kernel_address,kernel_size/PAGE_SIZE,0,1);
    

    //map_region(default_dir,(v_addr_t)&_physical_memory_free_address,(p_frame_t)&_physical_memory_free_address,1024,0,1);
    switch_directory(default_dir);
    kernel_directory = (page_directory_t *)GET_VIRTUAL_ADDRESS(0x300,0x3FE);
    kernel_tables_map = (page_table_t *)GET_VIRTUAL_ADDRESS(0x300,0x3FF);
    is_pagging_done = 1;
}

