#include <kernel/multiboot.h>
#include <kernel/bits.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/isr.h>
#include <kernel/syscalls.h>
#include <kernel/mem/pmm.h>
#include <kernel/mem/vmm.h>
#include <kernel/mem/kheap.h>
#include <kernel/drivers/vga.h>
#include <kernel/drivers/rtc.h>
#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/serial.h>
#include <kernel/drivers/vesa.h>
#include <kernel/drivers/pit.h>
#include <kernel/drivers/pci.h>
#include <kernel/drivers/ide.h>
#include <kernel/drivers/rtl8139.h>
#include <kernel/types.h>
#include <kernel/datastruct/list.h>
#include <kernel/datastruct/gtree.h>
#include <kernel/filesystems/vfs.h>
#include <kernel/filesystems/iso9660.h>
#include <kernel/system.h>
#include <logger.h>
#include <stdio.h>
#include <string.h>

void kernel_main(uint64_t magic, multiboot_info_t * mbi) 
{
  
  logger_install(LOG_LEVEL_TRACE);
  string_t logo = "\
   _____ ______   _____  ____      \n\r \
 | ____/ ___\\ \\ / / _ \\/ ___|  \n\r \
 |  _|| |  _ \\ V / | | \\___ \\  \n\r \
 | |__| |_| | | || |_| |___) |    \n\r \
 |_____\\____| |_| \\___/|____/   FUTURE IS NOW\n\r \
                                  \n\r \
  ";
  serial_print(COM1,logo);
  
  log_information("OS Starting");

  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
  {
    log_fatal("Invalid magic number: 0x%x\n", (uint32_t) magic);
    return;
  }

  // printf("addr: 0x%x, size: 0x%x, num: 0x%x",mbi->u.elf_sec.addr,mbi->u.elf_sec.size,mbi->u.elf_sec.num);
  if(BITREAD(mbi->flags,12)){
    vga_install(mbi->framebuffer_addr,mbi->framebuffer_width,mbi->framebuffer_height,mbi->framebuffer_bpp);
  }
  printf(logo);
  
  log_information("Installing GDT");
  gdt_install();
  log_information("GDT Installed Successfully");

  log_information("Installing IDT");
  idt_install();
  log_information("IDT Installed Successfully");

  log_information("Installing Real Time Clock RTC");
  rtc_install();
  log_information("Installing Real Time Clock RTC Installed Successfully");
  syscalls_install();
  // if(BITREAD(mbi->flags,11)){
  //   struct VbeInfoBlock * info_block = (struct VbeInfoBlock *)mbi->vbe_control_info;
  //   uint16_t * mode = (uint16_t *)((info_block->VideoModePtr[1]*0x10)+info_block->VideoModePtr[0]);
  //   string_t Oem = (string_t)((info_block->OemStringPtr[1]*0x10)+info_block->OemStringPtr[0]);
  //   while (*mode != 0xFFFF)
  //   {
  //     log_trace("mode: 0x%x",(uint32_t)*mode);
  //     mode++;
  //   }
  //   struct VbeModeInfoBlock * mdeblock = (struct VbeModeInfoBlock *)((mbi->vbe_interface_seg * 0x10)+mbi->vbe_interface_off);
  //   log_trace("VESA table available Version:%d, Total Memory:%d",(uint32_t)info_block->VbeVersion,(uint32_t)info_block->TotalMemory);
  // }
  if(BITREAD(mbi->flags,6)){
    pmm_install((multiboot_memory_map_t *)mbi->mmap_addr,mbi->mmap_length);
    log_information("Installing Memory Map .installed");
  }else{
    log_fatal("Cannot install memory FAILED");
    return;
  }
  
  vmm_install();
  log_information("Install Virtual Memory .installed");
  

  kheap_install();
  log_information("Install Kernel Heap .installed");


  keyboard_install();
  log_information("Install Keyboard Driver .installed");

  pit_install();
  log_information("Install Programable Interval Timer (PIT) .installed");
  
  vfs_install();
  log_information("Install Virtual File System (VFS) .installed");

  log_information("Install PCI.");
  pci_install();
  log_information("PCI Installed Successfully");

  log_information("Installing IDE Controller");
  pci_config_t * ide_controller = pci_get_device(0x1,0x1);
  ide_install(ide_controller);
  log_information("IDE Controller Installed Successfully");
  

  log_information("Installing ISO9660 FileSystem");
  iso9660_install("/dev/hda","/");
  log_information("ISO9660 FileSystem installed successfully");
  char date[50];
  str_date(date);
  log_information("current GMT Time Now %s",date);

  log_information("Installing Network Controller");
  pci_config_t * eth_controller = pci_get_device(0x2,0x0);
  rtl8139_install(eth_controller);
  log_information("Installing Network Controller Successfully");

  // ptr_t kmm = kmalloc(600);
  // memset(kmm,0xDD,0x200);
  // ide_write_sectors(1,1,0,kmm);
  // ide_read_sectors(1,1,0,kmm);
  // print_h();
  for (;;) { }
  

}
