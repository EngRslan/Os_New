#ifndef KHEAP_H
#define KHEAP_H
#include <stddef.h>
#include <kernel/types.h>
#define HEADER_SIZE (sizeof(kheap_block_header_t))
typedef struct kheap_block_header {
    uint32_t size;
    uint8_t free;
    struct kheap_block_header * next;
    struct kheap_block_header * prev;
}__attribute__ ((packed)) kheap_block_header_t ;

ptr_t kalloc(size_t size);
void kheap_install();
void kfree(ptr_t ptr);
#endif