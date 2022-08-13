#ifndef GDT_H
#define GDT_H

#define MAX_GDT_ENTRIES 8
typedef struct gdt_entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;

} __attribute__((packed)) gdt_entry_t;

typedef struct gdt
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed)) gdt_t;

void gdt_install(void);
void gdt_set_entry(int index,unsigned int base, unsigned int limit,unsigned char access, unsigned char gran);

#endif