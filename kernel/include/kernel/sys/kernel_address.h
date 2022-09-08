#ifndef KERNEL_ADDRESS_H
#define KERNEL_ADDRESS_H

extern unsigned int _virtual_kernel_address;
extern unsigned int _physical_kernel_address;
extern unsigned int _virtual_memory_bitmap_address;
extern unsigned int _physical_memory_bitmap_address;
extern unsigned int _virtual_memory_free_address ;
extern unsigned int _physical_memory_free_address;


/**
 *   @brief Virtual Kernel Memory Dir Index
 *   this is our kernel virtual address start
 *   this index is map to virtual address 0xC0000000
*/
#define KERNEL_DIR_INDX                  0x300
/**
 * @brief Default Kernel Directory Map Index
 * this page is map virtual address to default kernel directory to be able to read write into physical dir page
 * if the dir entry 0x300 so this value is map to virtual address 0xC03FE000
 * 
*/
#define KERNEL_DEFAULT_DIR_TABLE_INDX    0x3FE
/**
 * @brief Default Kernel 1024 Pages Tables map table
 * this page is to map every page table to be able to be able to map and unmape new address to physical tables from OS
 * if the dir entry 0x300 so this value is map to virtual address 0xC03FF000
 */
#define KERNEL_DIR_TABLES_TABLE_INDX     0x3FF
/**
 * @brief Kernel Map Tables
 * this virtual space used to map every physical page table to same index in virtual index
 * example : 
 * first page with index 0 is map to physical page address that contain the map entries for memory from address 0 to 4MG
 * and so on until index 1024 which map the physical page that map physical memory region from (4G-4M) to 4M
 * the first virtual address for this tables is 0xC0400000
 */

#define KERNEL_TABLES_MAP_DIR_INDX       0x301
/**
 * @brief Kernel heap start virtual address
 * this is the index of the kernel heap entry on the dir table
 * this map to virtual address 0xC0800000
 * every kmalloc function return value above this address
 */
#define KERNEL_HEAP_DIR_INDX             0x303
#endif