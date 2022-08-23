#include <kernel/sys/kernel_address.h>
#include <kernel/mem/pmm.h>
#include <kernel/mem/vmm.h>
#include <stddef.h>
#include <string.h>
// extern unsigned int _lower_kernel_end;
// extern unsigned int _higher_kernel_address;
// extern unsigned int _kernel_end;
page_directory_t * kernel_directory;
page_table_t * kernel_tables_map;

void map_page(page_directory_t * dir,v_addr_t virtual_address,p_frame_t physical_frame,unsigned int is_user,unsigned int is_writable);
void map_region(page_directory_t * dir, v_addr_t start,p_frame_t physical_frame,unsigned int total_pages,unsigned int is_user,unsigned int is_writable);
void flush_tlb_entry(v_addr_t address)
{
    __asm__ __volatile__ ("cli; invlpg (%0); sti" : : "r"(address) );
}
void free_page(page_directory_t * dir,v_addr_t virtual_address){
    page_table_directory_t * dir_entry = &dir->tables[DIR_INDEX(virtual_address)];
    if(!dir_entry->present){
        return;
    }

    page_table_t * pages_table = (page_table_t *)(dir_entry->frame << 12);
    page_table_entry_t * page_entry = &pages_table->pages[PAGE_INDEX(virtual_address)];
    if(!page_entry->present){
        return;
    }

    page_entry->present = 0;
    free_block((void *)(page_entry->frame << 12));
    flush_tlb_entry(virtual_address);
}
void allocate_page(page_directory_t * dir,v_addr_t virtual_address,unsigned int is_user,unsigned int is_writable){
    p_frame_t allocate_address = (p_frame_t) allocate_block();
    map_page(dir,virtual_address,allocate_address,is_user,is_writable);
}
void allocate_region(page_directory_t * dir,v_addr_t virtual_address,unsigned int total_pages,unsigned int is_user,unsigned int is_writable){
    
    for (unsigned int i = 0; i < total_pages; i++)
    {
        allocate_page(dir,virtual_address,is_user,is_writable);
        virtual_address += PAGE_SIZE;
    }
    
}

page_table_t * internal_alloc_page_table(page_directory_t * dir,unsigned int is_user,unsigned int is_writable){
    page_table_t * pt = (page_table_t *) allocate_block();
    
    unsigned int table_index = DIR_INDEX(pt);
    unsigned int page_index = PAGE_INDEX(pt);

    page_table_directory_t * page_dir_entry = &dir->tables[table_index];
    page_dir_entry->present = 1;
    page_dir_entry->rw = is_writable;
    page_dir_entry->user = is_user;
    page_dir_entry->frame = (unsigned int)pt >> 12;
    memset(pt,0,sizeof(page_table_t));
    return pt;
}

// page_table_t * get_or_create_table(page_directory_t * dir, v_addr_t virtual_address){
    
//     unsigned int table_index = DIR_INDEX(virtual_address);
//     unsigned int page_index = PAGE_INDEX(virtual_address);

//     page_table_directory_t * dir_entry = &dir->tables[table_index];
//     if(dir_entry->present){
//         return (page_table_t *)((unsigned int)dir_entry->frame << 12);
//     }
        
//     void * ph_addr = allocate_block();
//     dir_entry->present = 1;
//     dir_entry->rw = 1;
//     dir_entry->user = 0;
//     dir_entry->frame = (unsigned int)ph_addr >> 12;

//     page_table_directory_t * new_dir_entry = &dir->tables[DIR_INDEX(ph_addr)];
//     if(new_dir_entry->present)
//     {
//         return (page_table_t *)ph_addr;
//     }




// }
int is_pagging_done = 0;
void map_page(page_directory_t * dir,v_addr_t virtual_address,p_frame_t physical_frame,unsigned int is_user,unsigned int is_writable){
    page_table_t * pages_table = NULL;
    if(!dir){
        return ;
    }

    unsigned int table_index = DIR_INDEX(virtual_address);
    unsigned int page_index = PAGE_INDEX(virtual_address);

    page_table_directory_t * page_dir_entry = &dir->tables[table_index];

    if(!page_dir_entry->present){
        void * ph_page_table = (page_table_t *) allocate_block();
        
        page_table_entry_t * mapping_entry = &kernel_tables_map->pages[table_index];
        mapping_entry->present = 1;
        mapping_entry->rw = is_writable;
        mapping_entry->user = is_user;
        mapping_entry->frame = (unsigned int)ph_page_table >> 12;

        flush_tlb_entry((v_addr_t)kernel_tables_map);
        if(is_pagging_done){
            pages_table = (page_table_t *) ((0x301 << 22)|(table_index << 12)|0);
        }
        else{
            pages_table = (page_table_t *) ph_page_table;
        }
        page_dir_entry->present = 1;
        page_dir_entry->rw = is_writable;
        page_dir_entry->user = is_user;
        page_dir_entry->frame = (unsigned int)ph_page_table >> 12;
        memset(pages_table,0,sizeof(page_table_t));
    }else{
        if(is_pagging_done){
            pages_table = (page_table_t *) ((0x301 << 22)|(table_index << 12)|0);
        }
        else{
            pages_table = (page_table_t *)((unsigned int)page_dir_entry->frame << 12);
        }
        // pages_table = (page_table_t *)((unsigned int)page_dir_entry->frame << 12);
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
unsigned int virtual_kernel_heap_address = 0;
void vmm_install(){
    virtual_kernel_heap_address = (unsigned int)(&_virtual_memory_free_address + (1025*PAGE_SIZE));
    page_directory_t * default_dir = (page_directory_t *)allocate_block();
    memset(default_dir,0,sizeof(page_directory_t));

    kernel_tables_map = (page_table_t *)allocate_block();
    memset(kernel_tables_map,0,sizeof(page_table_t));
    
    // Map page tables space
    map_page(default_dir,(v_addr_t)((0x300<<0x16)|(0x3FF << 0xC) | 0x000),(p_frame_t)kernel_tables_map,0,1);
    page_table_directory_t * dir400 = &default_dir->tables[0x301];
    dir400->present = 1,
    dir400->user = 0;
    dir400->rw = 1;
    dir400->frame = (unsigned int)kernel_tables_map >> 12;

    //MAP PMM Bitmap Area
    // unsigned int pmm_size = bitmap_size/PAGE_SIZE;
    // if(pmm_size%PAGE_SIZE)pmm_size++;
    // map_region(default_dir,(v_addr_t)_higher_kernel_start,(p_frame_t)bitmap_start,pmm_size,0,1);
    //Map defualt directory
    map_page(default_dir,(v_addr_t)((0x300<<0x16)|(0x3FE << 0xC) | 0x000),(p_frame_t)default_dir,0,1);
    
    //map_page(default_dir,(v_addr_t)default_dir,(p_frame_t)default_dir,0,1);
    //Map First 4MB
    //map_region(default_dir,0,0,0x400000/PAGE_SIZE,0,1);
    map_region(default_dir,0xb8000,0xb8000,8,0,1);

    //map memory bitmap
    unsigned int map_size = (unsigned int)&_virtual_memory_free_address - (unsigned int)&_virtual_memory_bitmap_address;
    map_region(default_dir,(v_addr_t)&_physical_memory_bitmap_address,(p_frame_t)&_physical_memory_bitmap_address,map_size/PAGE_SIZE,0,1);

    // //map default kernel directory
    // map_page(default_dir,(v_addr_t)default_dir,(p_frame_t) default_dir,0,1);
    
    //MAP Kernel
    unsigned int kernel_size = (unsigned int)&_virtual_memory_bitmap_address - (unsigned int)&_virtual_kernel_address;
    map_region(default_dir,(v_addr_t)&_virtual_kernel_address,(unsigned int)&_physical_kernel_address,kernel_size/PAGE_SIZE,0,1);
    

    //map_region(default_dir,(v_addr_t)&_physical_memory_free_address,(p_frame_t)&_physical_memory_free_address,1024,0,1);
    switch_directory(default_dir);
    kernel_directory = (page_directory_t *)((0x300<<0x16)|(0x3FF << 0xC) | 0x000);
    kernel_tables_map = (page_table_t *)(769<<22);
    is_pagging_done = 1;
}

