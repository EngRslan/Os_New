#include <kernel/elf.h>
#include <stdbool.h>
#include <logger.h>
#include <stddef.h>


bool ElfCheckFile(Elf32Ehdr *hdr){
    if(!hdr) return false;
    if(hdr->e_ident[EI_MAG0] != ELFMAG0){
        log_error("ELF Header EI_MAG0 incorrect");
        return false;
    }
    if(hdr->e_ident[EI_MAG1] != ELFMAG1){
        log_error("ELF Header EI_MAG1 incorrect");
        return false;
    }
    if(hdr->e_ident[EI_MAG2] != ELFMAG2){
        log_error("ELF Header EI_MAG2 incorrect");
        return false;
    }
    if(hdr->e_ident[EI_MAG3] != ELFMAG3){
        log_error("ELF Header EI_MAG3 incorrect");
        return false;
    }

    return true;
}

bool ElfCheckSupported(Elf32Ehdr *hdr){
    if(!ElfCheckFile(hdr)){
        return false;
    }
    if(hdr->e_ident[EI_CLASS] != ELFCLASS32){
        log_error("Unsupported ELF File Class.");
        return false;
    }
    if(hdr->e_ident[EI_DATA] != ELFDATA2LSB){
        log_error("Unsupported ELF Byte Order.");
        return false;
    }
    if(hdr->e_machine != EM_386){
        log_error("Unsupported ELF File Target.");
        return false;
    }

    if(hdr->e_ident[EI_VERSION] != EV_CURRENT){
        log_error("Unsupported ELF File Version.");
        return false;
    }

    if(hdr->e_type != ET_REL && hdr->e_type != ET_EXEC){
        log_error("Unsupported ELF File Type.");
        return false;
    }
    return true;
}

// static inline void *elfLoadRel(Elf32Ehdr *hdr){
//     int result ;
//     result = elfLoadStage1(hdr);
//     if(result == ELF_RELOC_ERR){
//         log_error("Unable to load ELF file");
//         return NULL;
//     }
//     result = elfLoadStage2(hdr);
//     if(result == ELF_RELOC_ERR){
//         log_error("Unable to load ELF file");
//         return NULL;
//     }
//     return (void *)hdr->e_entry;
// }

static inline Elf32Shdr *elfSHeader(Elf32Ehdr *hdr){
    return (Elf32Shdr *)((int)hdr + hdr->e_shoff);
}

Elf32Shdr *elfSection(Elf32Ehdr *hdr,int idx){
    return &elfSHeader(hdr)[idx];
}

Elf32Sym *getSymEntry(Elf32Ehdr *hdr,int tab_idx,int idx){
    Elf32Shdr *shdr = elfSection(hdr,tab_idx);
    if(shdr->sh_type != SHT_SYMTAB){
        return NULL;
    }

    int totalEntries = shdr->sh_size/shdr->sh_entsize;
    if(idx>totalEntries){
        return NULL;
    }

    Elf32Sym *syment = (Elf32Sym *)((char *)hdr + shdr->sh_offset);
    return &syment[idx];
    
}
static inline char *elfStrTable(Elf32Ehdr *hdr){
    if(hdr->e_shstrndx == SHN_UNDEF) return NULL;
    return (char *)hdr+elfSection(hdr,hdr->e_shstrndx)->sh_offset;
}
char *elfLookupString(Elf32Ehdr *hdr,int offset){
    char *strtab = elfStrTable(hdr);
    if(strtab == NULL)return NULL;
    return strtab + offset;
}

void loadmodule(void *buffer){
   Elf32Ehdr * hdr = (Elf32Ehdr *)buffer;
   if(!ElfCheckSupported(hdr))
   {
    return;
   }

   
   
//    switch (hdr->e_type)
//    {
//     case ET_EXEC:
//         return NULL;
//     break;
//     case ET_REL:
//         return elfLoadRel(hdr);
//     default:
//         return NULL;
//     break;
//    }
}