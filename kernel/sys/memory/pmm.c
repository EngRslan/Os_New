#include <kernel/types.h>
#include <kernel/sys/kernel_address.h>
#include <kernel/mem/pmm.h>
#include <kernel/multiboot.h>
#include <string.h>
#include <stdio.h>

uint32_t find_free_block();
void mark_reserved_region(void * address,uint32_t len);
// uint32_t * kaddress = &_kernel_end-0xC0000000;
uint8_t * bitmap_start = (uint8_t *)&_physical_memory_bitmap_address;
void * memory_start = (void *)&_physical_memory_free_address ; 

uint32_t total_blocks ;
uint32_t bitmap_size ;

void pmm_install(multiboot_memory_map_t * address,uint32_t length){
    uint8_t mem_records = length / sizeof(multiboot_memory_map_t);
    multiboot_memory_map_t * last_memory_map = (multiboot_memory_map_t * )address+mem_records-1;
    
    total_blocks = (last_memory_map->addr+last_memory_map->len) / BLOCK_SIZE;
    bitmap_size = total_blocks / BLOCKS_PER_BUCKET;
    if(total_blocks % BLOCKS_PER_BUCKET)bitmap_size++;
    memset(bitmap_start,0,bitmap_size);
    // uint32_t alignedBitMap = (uint32_t)(bitmap_start+bitmap_size);
    // memory_start = (void *)BLOCK_ALIGN(alignedBitMap);

    for (uint8_t i = 0; i < mem_records; i++,address++)
    {
        // printf("\nmemory Addr:0x%x, Len:%x, Type:0x%x",(uint32_t)address->addr,(uint32_t)address->len,address->type);
        if(address->type != MULTIBOOT_MEMORY_AVAILABLE){
            mark_reserved_region((void *)((uint32_t)address->addr),(uint32_t)address->len);
        }
    }

    mark_reserved_region(0,(unsigned)&_physical_memory_free_address);
    
}

void * allocate_block(){
    uint32_t block = find_free_block();
    // if(block > 0){
        SETBIT(block);
        return (void *)(block*BLOCK_SIZE);
    // }
    // return (void * )0;
}
uint32_t find_free_block(){
    uint32_t i;
    for (i = 0; i < total_blocks; i++)
    {
        if(!ISSET(i))
            return i;
    }

    return 0;
    
}

void mark_reserved_region(void * address,uint32_t len){
    uint32_t total_blocks = len / BLOCK_SIZE;
    if((len % BLOCK_SIZE)>0)total_blocks++;
    uint32_t first_block_number = GET_BLOCK_NUMBER((uint32_t)address);

    for (uint32_t i = 0;i < total_blocks; i++)
    {
        SETBIT((first_block_number+i));
    }
    
}

void free_block(void * ptr){
    CLEARBIT((uint32_t)GET_BLOCK_NUMBER((uint32_t)ptr));
}