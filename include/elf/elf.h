#ifndef __ELF_H__
#define __ELF_H__

#include <stdint.h>

#define EI_NIDENT 16



// 32 bit types
#define Elf32_Addr  uint32_t
#define Elf32_Off   uint32_t
#define Elf32_Half  uint16_t
#define Elf32_Word  uint32_t
#define Elf32_Sword int32_t
#define Elf32_Char  uint8_t

typedef struct {
        Elf32_Char      e_ident[EI_NIDENT]; // elf identification
        Elf32_Half      e_type;
        Elf32_Half      e_machine;
        Elf32_Word      e_version;
        Elf32_Addr      e_entry;
        Elf32_Off       e_phoff;
        Elf32_Off       e_shoff;
        Elf32_Word      e_flags;
        Elf32_Half      e_ehsize;
        Elf32_Half      e_phentsize;
        Elf32_Half      e_phnum;
        Elf32_Half      e_shentsize;
        Elf32_Half      e_shnum;
        Elf32_Half      e_shstrndx;
} Elf32_Ehdr;

/////////////////////////////////////////////////////////
// 64 bit types
#define Elf64_Addr    uint64_t
#define Elf64_Off     uint64_t
#define Elf64_Half    uint16_t
#define Elf64_Word    uint32_t
#define Elf64_Sword   int32_t
#define Elf64_Xword   uint64_t
#define Elf64_Sxword  int64_t
#define Elf64_Char    uint8_t


typedef struct {
        Elf64_Char      e_ident[EI_NIDENT];
        Elf64_Half      e_type;
        Elf64_Half      e_machine;
        Elf64_Word      e_version;
        Elf64_Addr      e_entry;
        Elf64_Off       e_phoff;
        Elf64_Off       e_shoff;
        Elf64_Word      e_flags;
        Elf64_Half      e_ehsize;
        Elf64_Half      e_phentsize;
        Elf64_Half      e_phnum;
        Elf64_Half      e_shentsize;
        Elf64_Half      e_shnum;
        Elf64_Half      e_shstrndx;
} Elf64_Ehdr;


#endif
