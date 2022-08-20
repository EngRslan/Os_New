#include <kernel/mem/pmm.h>
#include <kernel/multiboot.h>
#include <string.h>
#include <stdio.h>

unsigned int find_free_block();
void mark_reserved_region(void * address,unsigned int len);
// unsigned int * kaddress = &_kernel_end-0xC0000000;
unsigned char * bitmap_start = ((unsigned char *)(&_kernel_end))-0xC0000000;
void * memory_start ; 

unsigned int total_blocks ;
unsigned int bitmap_size ;

void pmm_install(multiboot_memory_map_t * address,unsigned int length){
    unsigned char mem_records = length / sizeof(multiboot_memory_map_t);
    multiboot_memory_map_t * last_memory_map = (multiboot_memory_map_t * )address+mem_records-1;
    
    total_blocks = (last_memory_map->addr+last_memory_map->len) / BLOCK_SIZE;
    bitmap_size = total_blocks / BLOCKS_PER_BUCKET;
    if(total_blocks % BLOCKS_PER_BUCKET)bitmap_size++;
    memset(bitmap_start,0,bitmap_size);
    unsigned int alignedBitMap = (unsigned int)(bitmap_start+bitmap_size);
    memory_start = (void *)BLOCK_ALIGN(alignedBitMap);

    for (unsigned char i = 0; i < mem_records; i++,address++)
    {
        printf("\nmemory Addr:0x%x, Len:%x, Type:0x%x",(unsigned int)address->addr,(unsigned int)address->len,address->type);
        if(address->type != MULTIBOOT_MEMORY_AVAILABLE){
            mark_reserved_region((void *)((unsigned int)address->addr),(unsigned int)address->len);
        }
    }
    
}

void * allocate_block(){
    unsigned int block = find_free_block();
    // if(block > 0){
        SETBIT(block);
        return (void *)(memory_start + (block*BLOCK_SIZE));
    // }
    // return (void * )0;
}
unsigned int find_free_block(){
    unsigned int i;
    for (i = 0; i < total_blocks; i++)
    {
        if(!ISSET(i))
            return i;
    }

    return 0;
    
}

void mark_reserved_region(void * address,unsigned int len){
    unsigned int total_blocks = len / BLOCK_SIZE;
    if((len % BLOCK_SIZE)>0)total_blocks++;
    unsigned int first_block_number = GET_BLOCK_NUMBER((unsigned int)address);

    for (unsigned int i = 0;i < total_blocks; i++)
    {
        SETBIT((first_block_number+i));
    }
    
}

void free_block(void * ptr){
    CLEARBIT((unsigned int)GET_BLOCK_NUMBER((unsigned int)ptr));
}