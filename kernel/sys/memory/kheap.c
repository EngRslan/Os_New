#include <stddef.h>
#include <string.h>
#include <kernel/types.h>
#include <kernel/mem/kheap.h>
#include <kernel/mem/vmm.h>

kheap_block_header_t * kheap_address =  (kheap_block_header_t *)GET_VIRTUAL_ADDRESS(KERNEL_HEAP_DIR_INDX,0x0);
unsigned int total_allocated_pages = 0;
ptr_t allocate_additional_page();

void kheap_install(){
    kheap_block_header_t * startup_heap = (kheap_block_header_t *)allocate_additional_page();
    startup_heap->free = 1;
    startup_heap->prev = NULL;
    startup_heap->next = NULL;
    startup_heap->size = PAGE_SIZE - HEADER_SIZE;
}


ptr_t allocate_additional_page(){
    v_addr_t page_addr = (v_addr_t)((ptr_t)kheap_address + (total_allocated_pages * PAGE_SIZE));
    kallocate_page(page_addr);
    // memset(page_addr,0,PAGE_SIZE);
    total_allocated_pages ++;
    return (ptr_t)page_addr;
}

kheap_block_header_t * find_free_space(size_t size){
    kheap_block_header_t * current_node = kheap_address;
    kheap_block_header_t * last_node;

    do{
        if(current_node->free && current_node->size >= size){
            return current_node;
        }
        last_node = current_node;
        current_node = current_node->next;
    }
    while (current_node != NULL);
    uint32_t pages_needed = size / PAGE_SIZE;
    if(size % PAGE_SIZE)pages_needed++;

    for (; pages_needed > 0; pages_needed --)
    {
        allocate_additional_page();
        last_node->size += PAGE_SIZE;
    }

    return last_node;
}
void split_block_space(kheap_block_header_t * space, size_t size){
    kheap_block_header_t * new_space = (kheap_block_header_t * )((void *)space+size+HEADER_SIZE);

    new_space->prev = space;
    new_space->next = space->next;
    new_space->size = space->size - size - HEADER_SIZE;
    new_space->free = 1;

    space->next = new_space;
    space->size = size;
    space->free = 0;    
}
ptr_t kmalloc(size_t size){
    if(size == 0){
        return NULL;
    }
    kheap_block_header_t * free_space = find_free_space(size + HEADER_SIZE);
    split_block_space(free_space,size);
    return (ptr_t)(ptr_t)free_space + HEADER_SIZE;
}

void kfree(ptr_t ptr){
    kheap_block_header_t * node = ptr-HEADER_SIZE;
    if (node->free) return;

    if(node->next && node->next->free){
        node->size += node->next->size + HEADER_SIZE;
        node->next = node->next->next;
    }
    
    if(node->prev && node->prev->free){
        node->prev->size += node->size+HEADER_SIZE;
        node->prev->next = node->next;
        node = node->prev;
    }

    node->free = 1;
    memset((ptr_t)node + HEADER_SIZE,0,node->size);
}

