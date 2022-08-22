/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)
 
/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Begin putting sections at 1 MiB, a conventional place for kernels to be
	   loaded at by the bootloader. */
	. = 1M;
	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	_lower_kernel_address = .;
	.multiboot.data :
	{
		*(.multiboot.data)
	}
	.lower_half.text :{
		*(.lower_half.text)
	}
	.lower_half.bss :{
		*(.lower_half.bss)
	}
    _physical_kernel_address = .;
	. = 3072M;
	_virtual_kernel_address = . ;
	.text BLOCK(4K) :  AT(ADDR(.text) - 3072M + _physical_kernel_address)
	{
		*(.text)
	}
 
	/* Read-only data. */
	.rodata BLOCK(4K) : AT(ADDR(.rodata) - 3072M + _physical_kernel_address)
	{
		*(.rodata)
	}
 
	/* Read-write data (initialized) */
	.data BLOCK(4K) : AT(ADDR(.data) - 3072M + _physical_kernel_address)
	{
		*(.data)
	}
 
	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : AT(ADDR(.bss) - 3072M + _physical_kernel_address)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}
	. = ALIGN(4K) ;
	_virtual_memory_bitmap_address = . ;
	_physical_memory_bitmap_address = . - _virtual_kernel_address + _physical_kernel_address;
	. += (4096M/4K)/8;
	_virtual_memory_free_address = .;
	_physical_memory_free_address = . - _virtual_kernel_address + _physical_kernel_address;
}