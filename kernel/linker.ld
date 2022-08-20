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
	_kernel_start = .;
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
	_lower_kernel_end = .;
	. = 3072M;
	_higher_kernel_start = . ;
	.text BLOCK(4K) :  AT(ADDR(.text) - 3072M + _lower_kernel_end)
	{
		*(.text)
	}
 
	/* Read-only data. */
	.rodata BLOCK(4K) : AT(ADDR(.rodata) - 3072M + _lower_kernel_end)
	{
		*(.rodata)
	}
 
	/* Read-write data (initialized) */
	.data BLOCK(4K) : AT(ADDR(.data) - 3072M + _lower_kernel_end)
	{
		*(.data)
	}
 
	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : AT(ADDR(.bss) - 3072M + _lower_kernel_end)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}
	_kernel_end = .;
	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}