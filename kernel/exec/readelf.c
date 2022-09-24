#include <kernel/elf.h>
#include <stdbool.h>
#include <logger.h>
#include <stddef.h>
#include <stdio.h>

extern bool ElfCheckSupported(Elf32Ehdr *hdr);
extern char *elfLookupString(Elf32Ehdr *hdr,int offset);
extern Elf32Shdr *elfSection(Elf32Ehdr *hdr,int idx);
extern Elf32Sym *getSymEntry(Elf32Ehdr *hdr,int tab_idx,int idx);

const char *secTypes[]={
    "NULL",
    "PROGBITS","SYMTAB","STRTAB",
    "RELA","HASH","DYNAMIC",
    "NOTE","NOBITS","REL",
    "SHLIB","DYNSYM"
    };
const char *StbBindings[]={
    "LOCAL",
    "GLOBAL",
    "WEAK",
    [13]="LOPROC",
    [15]="HIPROC"
};

const char *StbTypes[]={
    "NOTYPE",
    "OBJECT",
    "FUNC",
    "SECTION",
    "FILE",
    [13]="LOPROC",
    [15]="HIPROC"
};

const char *RtTTypes[]={
    "R_386_NONE",
    "R_386_32",
    "R_386_PC32"
};

const char *EMachine[]={
    "No machine",
    "AT&T WE 32100",
    "SPARC",
    "Intel Architecture",
    "Motorola 68000",
    "Motorola 88000",
    "",
    "Intel 80860",
    "MIPS RS3000 Big-Endian",
    "MIPS RS4000 Big-Endian",
    [0x14]="PowerPC",
    [0x28]="ARM",
    [0x2A]="SuperH",
    [0x32]="IA-64",
    [0x3E]="x86-64",
    [0xB7]="AArch64",
    [0xF3]="RISC-V"
};

const char* PhTypes[]={
    "NULL",
    "LOAD",
    "DYNAMIC",
    "INTERP",
    "NOTE",
    "SHLIB",
    "PHDR"
};

char *secFlag(Elf32Word flag){
    uint8_t cidx = 0;
    static char flg[3];
    if(flag == 0){
        flg[0]=' ';
        flg[1]=0x0;
        return flg;
    }
    for (size_t i = 0; i < sizeof(flag) * 4; i++)
    {
        uint32_t isSet = flag & (1 << i);

        switch (isSet)
        {
        case SHF_WRITE:
            flg[cidx++] = 'W';
            break;
        case SHF_ALLOC:
            flg[cidx++] = 'A';
            break;
        case SHF_EXECINSTR:
            flg[cidx++] = 'X';
            break;
        case SHF_MERGE:
            flg[cidx++] = 'M';
            break;
        case SHF_STRINGS:
            flg[cidx++] = 'S';
            break;
        case SHF_INFO_LINK:
            flg[cidx++] = 'I';
            break;
        case SHF_LINK_ORDER:
            flg[cidx++] = 'L';
            break;
        case SHF_OS_NONCONFORMING:
            flg[cidx++] = 'O';
            break;
        case SHF_GROUP:
            flg[cidx++] = 'G';
            break;
        case SHF_TLS:
            flg[cidx++] = 'T';
            break;
        default:
            if(isSet>0)
                flg[cidx++] = 'x';
            break;
        }
    }
    flg[cidx] = 0x0;
    return flg;
}
char *specialSI(Elf32Half ndx){
    static char str[4];
    uint8_t i = 0;

    switch (ndx)
    {
    case SHN_UNDEF:
        str[i++]='U';
        str[i++]='N';
        str[i++]='D';
        str[i++]=0;
        break;
    case SHN_ABS:
        str[i++]='A';
        str[i++]='B';
        str[i++]='S';
        str[i++]=0;
    break;
    case SHN_COMMON:
        str[i++]='C';
        str[i++]='M';
        str[i++]='N';
        str[i++]=0;
    break;
    default:
        sprintf(str,"%3x",(uint32_t)ndx);
        break;
    }

    return str;
}
void printSectionHeaders(Elf32Ehdr *hdr){
    log_information(" ");
    log_information("Section Headers:");
    log_information("[Nr] Name\033[33GType\033[49GAddr\033[58GOff\033[65GSize\033[72GES\033[75GFlg\033[79GLK\033[82GInf\033[86GAl");
    for (int i = 0; i < hdr->e_shnum; i++)
    {
        Elf32Shdr *sec = elfSection(hdr,i);
        log_information("[%2d] %s\033[33G%s\033[49G%08x\033[58G%06x\033[65G%06x\033[72G%02x\033[75G%2s\033[79G%d\033[82G%d\033[86G%d",
            (uint32_t)i,elfLookupString(hdr,sec->sh_name),secTypes[sec->sh_type],sec->sh_addr,sec->sh_offset,sec->sh_size,sec->sh_entsize,secFlag(sec->sh_flags),sec->sh_link,sec->sh_info,sec->sh_addralign);
    }
}

void printRelSection(Elf32Ehdr *hdr,Elf32Shdr *sec){
    int reltab_entries = sec->sh_size/sec->sh_entsize;
    int relAddr = (int)hdr + sec->sh_offset;
    log_information(" ");
    log_information("Relocation section '%s' at offset 0x%x contains %d entries:",elfLookupString(hdr,sec->sh_name),sec->sh_offset,reltab_entries);
    log_information("Offset\033[22GInfo\033[31GType\033[45GSym.Value\033[56GSym. Name");
    for (int j = 0; j < reltab_entries; j++)
    {
        Elf32Rel *relentry = &((Elf32Rel *)relAddr)[j];
        getSymEntry(hdr,sec->sh_link,ELF32_R_SYM(relentry->r_info));
        Elf32Sym *entry = getSymEntry(hdr,sec->sh_link,ELF32_R_SYM(relentry->r_info));
        Elf32Shdr *sth = elfSection(hdr,entry->st_shndx);
        char *name = elfLookupString(hdr,sth->sh_name);
        log_information("%08x\033[22G%08x\033[31G%s\033[45G%08x\033[56G%s",relentry->r_offset,relentry->r_info,(uint32_t)RtTTypes[ELF32_R_TYPE(relentry->r_info)],(uint32_t)entry->st_value,name);
    }
}

void printSymSection(Elf32Ehdr *hdr,Elf32Shdr *sec){
    int symtab_entries = sec->sh_size/sec->sh_entsize;
    int symAddr = (int)hdr + sec->sh_offset;
    Elf32Shdr *strtab = elfSection(hdr,sec->sh_link);
    log_information(" ");
    log_information("Symbol table '%s' contains %d entries:",elfLookupString(hdr,sec->sh_name),symtab_entries);
    log_information("Num:\033[18GValue\033[25GSize\033[30GType\033[38GBind\033[45GVis\033[54GNdx\033[58GName");
    for (int j = 0; j < symtab_entries; j++)
    {
        Elf32Sym *sentry = &((Elf32Sym *)symAddr)[j];
        char * name = (char *)hdr + strtab->sh_offset + sentry->st_name;
        log_information("%3d:\033[15G%08x\033[25G%4d\033[30G%s\033[38G%s\033[45G%s\033[54G%s\033[58G%s",j,sentry->st_value,sentry->st_size,StbTypes[ELF32_ST_TYPE(sentry->st_info)] ,StbBindings[ELF32_ST_BIND(sentry->st_info)],"DEFAULT",specialSI(sentry->st_shndx),name);
    }
}
void printSection(Elf32Ehdr *hdr,Elf32Shdr *sec){
    switch (sec->sh_type)
    {
    case SHT_REL:
        printRelSection(hdr,sec);
        break;
    
    case SHT_SYMTAB:
        printSymSection(hdr,sec);
        break;
    }
}
void printProgramHeaders(Elf32Ehdr *hdr){
    Elf32Phdr *phdrbase = (Elf32Phdr *)((char *)hdr + hdr->e_phoff);
    log_information(" ");
    log_information("Program Headers:");
    log_information("\033[12GType\033[27GOffset\033[36GVirtAddr\033[47GPhysAddr\033[58GFileSiz\033[66GMemSiz\033[74GFlg\033[78GAlign");
    for (int i = 0; i < hdr->e_phnum; i++)
    {
        Elf32Phdr *phdr = &phdrbase[i];
        log_information("\033[12G%s\033[27G0x%06x\033[36G0x%08x\033[47G0x%08x\033[58G0x%05x\033[66G0x%05x\033[74G%3s\033[78G0x%04x",
            PhTypes[phdr->p_type],phdr->p_offset,phdr->p_vaddr,phdr->p_paddr,phdr->p_filesz,phdr->p_memsz,secFlag(phdr->p_flags),phdr->p_align);
    }
    
}
void printElfHeader(Elf32Ehdr *hdr){
    log_information("ELF Header:");
    log_information("  Magic: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
        (uint32_t)hdr->e_ident[0],(uint32_t)hdr->e_ident[1],(uint32_t)hdr->e_ident[2],(uint32_t)hdr->e_ident[3],
        (uint32_t)hdr->e_ident[4],(uint32_t)hdr->e_ident[5],(uint32_t)hdr->e_ident[6],(uint32_t)hdr->e_ident[7],
        (uint32_t)hdr->e_ident[8],(uint32_t)hdr->e_ident[9],(uint32_t)hdr->e_ident[10],(uint32_t)hdr->e_ident[11],
        (uint32_t)hdr->e_ident[12],(uint32_t)hdr->e_ident[13],(uint32_t)hdr->e_ident[14],(uint32_t)hdr->e_ident[15]);
    log_information("  CLASS:\033[47G%s",(const char *[]){"INVALID","ELF32","ELF64"}[hdr->e_ident[EI_CLASS]]);
    log_information("  Data:\033[47G%s",(const char *[]){"INVALID","little endian","Big endian"}[hdr->e_ident[EI_DATA]]);
    log_information("  Version:\033[47G%d%s",hdr->e_ident[EI_VERSION],hdr->e_ident[EI_VERSION] == EV_CURRENT?" (current)":" ");
    log_information("  OS/ABI:\033[47G%s",hdr->e_ident[EI_ABIVERSION] == 0?"UNIX - System V":"Unknown");
    log_information("  ABI Version:\033[47G%d",hdr->e_ident[EI_ABIVERSION]);
    log_information("  Type:\033[47G%s",(const char *[]){"No Type","REL (Relocatable file)","EXEC (Executable file)","SHRD (Shared object file)","CORE (Core file)"}[hdr->e_type]);
    log_information("  Machine:\033[47G%s",EMachine[hdr->e_machine]);
    log_information("  Entry point address:\033[47G0x%x",hdr->e_entry);
    log_information("  Start of program headers:\033[47G%d (bytes into file)",hdr->e_phoff);
    log_information("  Start of section headers:\033[47G%d (bytes into file)",hdr->e_shoff);
    log_information("  Flags:\033[47G0x%x",hdr->e_flags);
    log_information("  Size of this header:\033[47G%d (bytes)",hdr->e_ehsize);
    log_information("  Size of program headers:\033[47G%d (bytes)",hdr->e_phentsize);
    log_information("  Number of program headers:\033[47G%d", hdr->e_phnum);
    log_information("  Size of section headers:\033[47G%d (bytes)", hdr->e_shentsize);
    log_information("  Number of section headers:\033[47G%d", hdr->e_shnum);
    log_information("  Section header string table index:\033[47G%d",(uint32_t) hdr->e_shstrndx);


}
void ReadElf(void *buffer){
    Elf32Ehdr * hdr = (Elf32Ehdr *)buffer;
    if(!ElfCheckSupported(hdr))
    {
        return;
    }
    printElfHeader(hdr);
    printSectionHeaders(hdr);
    printProgramHeaders(hdr);
    for (int i = 0; i < hdr->e_shnum; i++)
    {
        Elf32Shdr *sec = elfSection(hdr,i);
        printSection(hdr,sec);
        
    }
}