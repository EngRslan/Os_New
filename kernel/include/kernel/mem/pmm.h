#ifndef PMM_H
#define PMM_H
    #define BLOCK_SIZE 4096
    #define BLOCKS_PER_BUCKET 8

    #define SETBIT(i) bitmap_start[i / BLOCKS_PER_BUCKET] = bitmap_start[i / BLOCKS_PER_BUCKET] | (1 << (i % BLOCKS_PER_BUCKET))
    #define CLEARBIT(i) bitmap_start[i / BLOCKS_PER_BUCKET] = bitmap_start[i / BLOCKS_PER_BUCKET] & (~(1 << (i % BLOCKS_PER_BUCKET)))
    #define ISSET(i) ((bitmap_start[i / BLOCKS_PER_BUCKET] >> (i % BLOCKS_PER_BUCKET)) & 0x1)
    #define GET_BUCKET32(i) (*((uint32_t*) &bitmap_start[i / 32]))

    #define BLOCK_ALIGN(addr) (((addr) & 0xFFFFF000) + 0x1000)

    extern unsigned int kernel_end;

    void pmm_install();
    void * allocate_block();
    void free_block(void * ptr);
#endif