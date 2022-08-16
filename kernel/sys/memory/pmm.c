#include <kernel/mem/pmm.h>
#include <kernel/multiboot.h>
#include <string.h>

unsigned int find_free_block();
void mark_reserved_region(unsigned int address,unsigned int len);

unsigned int * bitmap_start = (unsigned int *)&kernel_end;
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
    unsigned int alignedBitMap = (unsigned int)((void *)bitmap_start+bitmap_size);
    memory_start = (void *)BLOCK_ALIGN(alignedBitMap);

    for (unsigned char i = 0; i < mem_records; i++,address++)
    {
        if(address->type != MULTIBOOT_MEMORY_AVAILABLE){
            mark_reserved_region(address->addr,address->len);
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

void mark_reserved_region(unsigned int address,unsigned int len){
    unsigned int total_blocks = len / BLOCK_SIZE;
    if((len % BLOCK_SIZE)>0)total_blocks++;
    // align address to start of block
    unsigned int start_address = address / BLOCK_SIZE ;
    // unsigned int alignedBlock = start_address / BLOCK_SIZE;

    for (;total_blocks > 0; total_blocks--)
    {
        SETBIT(start_address+total_blocks);
    }
    
}

void free_block(void * ptr){
    CLEARBIT((unsigned int)((ptr - memory_start)/BLOCK_SIZE));
}