#include <kernel/mem/pmm.h>
#include <string.h>

extern unsigned int kernal_start;

void * kernal_start_addr = (void *)&kernal_start;

void * bitmap_start = (void *)&kernal_end;

void * memory_start ;

unsigned int total_blocks ;
unsigned int bitmap_size ;

void pmm_install(){
    unsigned int kernel_size = bitmap_start - kernal_start_addr;
    
    total_blocks = (1096*1024*1024) / BLOCK_SIZE;
    bitmap_size = total_blocks / BLOCKS_PER_BUCKET;
    if(total_blocks % BLOCKS_PER_BUCKET)bitmap_size++;
    memset(bitmap_start,0,bitmap_size);
    memory_start = (void *)BLOCK_ALIGN((unsigned int)(bitmap_start+bitmap_size));
}