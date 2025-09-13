#ifndef __ELF_COMMON_H__
#define __ELF_COMMON_H__

#include <stdint.h>

// common types
#define Elf_Byte    unsigned char
// 32 bit types
#define Elf32_Addr  uint32_t
#define Elf32_Off   uint32_t
#define Elf32_Half  uint16_t
#define Elf32_Word  uint32_t
#define Elf32_Sword int32_t

// 64 bit types
#define Elf64_Addr    uint64_t
#define Elf64_Off     uint64_t
#define Elf64_Half    uint16_t
#define Elf64_Word    uint32_t
#define Elf64_Sword   int32_t
#define Elf64_Xword   uint64_t
#define Elf64_Sxword  int64_t


// special indexes
#define SHN_UNDEF     0
#define SHN_LORESERVE 0xff00
#define SHN_LOPROC    0xff00
#define SHN_HIPROC    0xff1f
#define SHN_LOOS      0xff20
#define SHN_HIOS      0xff3f
#define SHN_ABS       0xfff1
#define SHN_COMMON    0xfff2
#define SHN_XINDEX    0xffff
#define SHN_HIRESERVE 0xffff

// elf class
typedef enum : Elf_Byte {
  ELFCLASSNONE,
  ELFCLASS32,
  ELFCLASS64
} elf_class_e;


// ELF file struct
#define Elf_Ehdr void
#define Elf_Shdr void
#define Elf_Phdr void

typedef struct {
  char            *name;
  Elf_Ehdr        *header;
  Elf_Shdr        *sht; 
  uint64_t        sh_num;
  Elf_Phdr        *pht;
  elf_class_e     type;
  void            *sections;
} elf_s;


#endif
