#include <kernel/types.h>
#include <kernel/gdt.h>
#include <string.h>

extern void gdt_load(uint32_t gdt_addr);

gdt_entry_t gdt_entries[MAX_GDT_ENTRIES];
gdt_t gdt;

void gdt_install(void){
    memset(&gdt_entries,0x0,sizeof(gdt_entries));
    memset(&gdt,0x0,sizeof(gdt_t));

    gdt.limit = sizeof(gdt_entries) - 1;
    gdt.base = (uint32_t)gdt_entries;
    
    //Set Null Descriptior
    gdt_set_entry(0,0,0,0,0);

    
    //Index 1
    //Start of memory 0
    //Length of memory from start 4GB
    //Access 
    //  1   => Present bit 0 = not present, 1= present
    //  00  => Descriptor Privilege level 0 = highest 3 = lowest
    //  1   => Descriptor Type 0 = defines a system segment (eg. a Task State Segment), 1 =  defines a code or data segment.
    //  1   => Executable bit 0 = Data Segment, 1=Code segment
    //  0   => Direction bit/Conforming bit.
    //         Is Data Segment ? 0 = segment is grows up, 1 = segment is grows down
    //         Is Code Segment ? 0 = Code only be exected from the ring set before in second and thired bits, 1= code can be executed from equal or lower
    //  1   => Read/Write 
    //          Is Data Segment ? 0 = read only, 1= read write access
    //          Is Code Segemnt ? 0 = no read , 1= read
    //  0   => Accessed (CPU Use) 0 = CPU Not Access this segment yet, 1= cpu access this segment
    //Gran (High 4 bits)
    //  1   => Granularity flag 0 limit is 1 byte block, 1= limit is 4 KiB blocks(page)
    //  1   => Size Flag 0= 16bit protected mode, 1= 32bit protected mode
    //  0   => Long Mode code flag 0= not 64bit code segment, 1= 64bit code segment
    //  0   => Reserved
    //  1111=> limit 4bit extenstion (1111 for bitwise and inside set);

    //Set Kernel Code
    gdt_set_entry(1,0,0xFFFFFFFF,0b10011010,0b11001111);

    //Set Kernel Data
    gdt_set_entry(2,0,0xFFFFFFFF,0b10010010,0b11001111);

    //Set User Code
    gdt_set_entry(3,0,0xFFFFFFFF,0b11111010,0b11001111);

    //Set User Data
    gdt_set_entry(4,0,0xFFFFFFFF,0b11110010,0b11001111);

    gdt_load((uint32_t)&gdt);
}


void gdt_set_entry(int index,uint32_t base, uint32_t limit,uint8_t access, uint8_t gran){
    gdt_entry_t * entry = &gdt_entries[index];

    // Set Entry Base 
    entry->base_low = base & 0xFFFF; // First 16 bit
    entry->base_middle = (base >> 16) & 0xFF; // next 8 bit
    entry->base_high = (base >> 24) & 0xFF; // next 8 bit

    // Set Limit
    entry->limit_low = limit & 0xFFFF; // First 16 bit of limit
    entry->granularity = (limit >> 16) & 0x0F; // next 4 bit of limit into fist 4 bit of granularity

    //set Access
    entry->access = access;

    //set granularity
    //first 4 bit is the limit set above
    entry->granularity = entry->granularity | (gran & 0xF0);

}