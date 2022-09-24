#ifndef ELF_H
#define ELF_H
#include <kernel/types.h>
#define ELFMAG0     0x7F
#define ELFMAG1     'E'
#define ELFMAG2     'L'
#define ELFMAG3     'F'
#define ELFDATA2LSB (1)
#define ELFCLASS32  (1)
typedef uint16_t Elf32Half;
typedef uint32_t Elf32Off;
typedef uint32_t Elf32Addr;
typedef uint32_t Elf32Word;
typedef int32_t Elf32SWord;
enum ElfIdent{
    EI_MAG0 = 0,
    EI_MAG1 = 1,
    EI_MAG2 = 2,
    EI_MAG3 = 3,
    EI_CLASS = 4,
    EI_DATA = 5,
    EI_VERSION = 6,
    EI_OSABI = 7,
    EI_ABIVERSION = 8,
    EI_PAD = 9
};

enum ElfMachine
{
    EM_386 = 3
};

enum ElfVersion
{
    EV_CURRENT = 1
};

enum ElfType
{
    ET_REL = 1,
    ET_EXEC = 2
};

# define SHN_UNDEF	(0x00) // Undefined/Not present
# define SHN_LORESERVE	(0xFF00) // Undefined/Not present
# define SHN_LOPROC	(0xFF00) // Undefined/Not present
# define SHN_HIPROC	(0xFF1F) // Undefined/Not present
# define SHN_ABS	(0xFFF1) // Undefined/Not present
# define SHN_COMMON	(0xFFF2) // Undefined/Not present
# define SHN_HIRESERVE	(0xFFFF) // Undefined/Not present

#define ELF_RELOC_ERR -1
enum ShTTypes {
	SHT_NULL	    = 0,   
	SHT_PROGBITS	= 1,   
	SHT_SYMTAB	    = 2,
	SHT_STRTAB	    = 3,
	SHT_RELA	    = 4,
	SHT_HASH	    = 5,
	SHT_DYNAMIC	    = 6,
	SHT_NOTE	    = 7,
	SHT_NOBITS	    = 8,
	SHT_REL		    = 9,
    SHT_SHLIB       = 10,
    SHT_DYNSYM      = 11
};
 
enum ShTAttributes {
	SHF_WRITE	        = 0x01, // Writable section
	SHF_ALLOC	        = 0x02,  // Exists in memory
    SHF_EXECINSTR       = 0x04,
    SHF_MERGE           = 0x10,
    SHF_STRINGS         = 0x20,
    SHF_INFO_LINK       = 0x40,
    SHF_LINK_ORDER      = 0x80,
    SHF_OS_NONCONFORMING = 0x100,
    SHF_GROUP           = 0x200,
    SHF_TLS             = 0x400
};

# define ELF32_ST_BIND(INFO)	((INFO) >> 4)
# define ELF32_ST_TYPE(INFO)	((INFO) & 0x0F)
enum StTBindings{
    STB_LOCAL		= 0,
    STB_GLOBAL		= 1,
    STB_WEAK		= 2,
    STB_LOPROC      = 13,
    STB_HIPROC      = 15,
};
enum StTTypes {
	STT_NOTYPE		= 0, 
	STT_OBJECT		= 1, 
	STT_FUNC		= 2,
    STT_SECTION     = 3,
    STT_FILE        = 4,
    STT_LOPROC      = 13,
    STT_HIPROC      = 15,
};

typedef struct
{
    uint8_t     e_ident[16];
    Elf32Half	e_type;
	Elf32Half	e_machine;
	Elf32Word	e_version;
	Elf32Addr	e_entry;
	Elf32Off	e_phoff;
	Elf32Off	e_shoff;
	Elf32Word	e_flags;
	Elf32Half	e_ehsize;
	Elf32Half	e_phentsize;
	Elf32Half	e_phnum;
	Elf32Half	e_shentsize;
	Elf32Half	e_shnum;
	Elf32Half	e_shstrndx;
} Elf32Ehdr;

typedef struct
{
    Elf32Word	sh_name;
	Elf32Word	sh_type;
	Elf32Word	sh_flags;
	Elf32Addr	sh_addr;
	Elf32Off	sh_offset;
	Elf32Word	sh_size;
	Elf32Word	sh_link;
	Elf32Word	sh_info;
	Elf32Word	sh_addralign;
	Elf32Word	sh_entsize;
}Elf32Shdr;

typedef struct {
	Elf32Word		st_name;
	Elf32Addr		st_value;
	Elf32Word		st_size;
	uint8_t			st_info;
	uint8_t			st_other;
	Elf32Half		st_shndx;
} Elf32Sym;

enum RtTTypes {
	R_386_NONE		= 0, // No relocation
	R_386_32		= 1, // Symbol + Offset
	R_386_PC32		= 2  // Symbol + Offset - Section Offset
};

# define ELF32_R_SYM(INFO)	((INFO) >> 8)
# define ELF32_R_TYPE(INFO)	((uint8_t)(INFO))
typedef struct {
	Elf32Addr		r_offset;
	Elf32Word		r_info;
} Elf32Rel;
 
typedef struct {
	Elf32Addr		r_offset;
	Elf32Word		r_info;
	Elf32SWord		r_addend;
} Elf32Rela;

enum PtTypes{
    PT_NULL = 0,
    PT_LOAD,
    PT_DYNAMIC,
    PT_INTERP,
    PT_NOTE,
    PT_SHLIB,
    PT_PHDR
};

typedef struct 
{
    Elf32Word p_type;
    Elf32Off p_offset;
    Elf32Addr p_vaddr;
    Elf32Addr p_paddr;
    Elf32Word p_filesz;
    Elf32Word p_memsz;
    Elf32Word p_flags;
    Elf32Word p_align;
} Elf32Phdr;

void loadmodule(void *buffer);
void ReadElf(void *buffer);
#endif