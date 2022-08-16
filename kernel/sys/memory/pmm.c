#include <kernel/mem/pmm.h>
#include <string.h>
unsigned int find_free_block();

unsigned int * bitmap_start = (unsigned int *)&kernel_end;
void * memory_start ;

unsigned int total_blocks ;
unsigned int bitmap_size ;

void pmm_install(){
    total_blocks = (1096*1024*1024) / BLOCK_SIZE;
    bitmap_size = total_blocks / BLOCKS_PER_BUCKET;
    if(total_blocks % BLOCKS_PER_BUCKET)bitmap_size++;
    memset(bitmap_start,0,bitmap_size);
    unsigned int alignedBitMap = (unsigned int)(bitmap_start+bitmap_size);
    memory_start = (void *)alignedBitMap;
}

void * allocate_block(){
    unsigned int block = find_free_block();
    if(block > 0){
        SETBIT(block);
        return (void *)block;
    }
    return (void * )0;
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

void free_block(void * ptr){
    CLEARBIT((unsigned int)ptr);
}