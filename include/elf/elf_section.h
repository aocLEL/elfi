#ifndef __ELF_SECTION_H__
#define __ELF_SECTION_H__

#include <stdint.h>
#include "elf_common.h"

// section header structures

// section attribute flags
#define SHF_WRITE             0x1
#define SHF_ALLOC             0x2
#define SHF_EXECINSTR         0x4
#define SHF_MERGE             0x10
#define SHF_STRINGS           0x20
#define SHF_INFO_LINK         0x40
#define SHF_LINK_ORDER        0x80
#define SHF_OS_NONCONFORMING  0x100
#define SHF_GROUP             0x200
#define SHF_TLS               0x400
#define SHF_COMPRESSED        0x800
#define SHF_MASKOS            0x0ff00000
#define SHF_MASKPROC          0xf0000000
// group flags
#define GRP_COMDAT            0x1 
#define GRP_MASKOS            0x0ff00000
#define GRP_MASKPROC          0xf0000000

// section type
typedef enum : uint32_t {
  SHT_NULL,
  SHT_PROGBITS,
  SHT_SYMTAB,
  SHT_STRTAB,
  SHT_RELA,
  SHT_HASH,
  SHT_DYNAMIC,
  SHT_NOTE,
  SHT_NOBITS,
  SHT_REL,
  SHT_SHLIB,
  SHT_DYNSYM,
  SHT_INIT_ARRAY      = 14,
  SHT_FINI_ARRAY,
  SHT_PREINIT_ARRAY,
  STH_GROUP,
  SHT_SYMTAB_SHNDX,
  SHT_LOOS            = 0x60000000,
  SHT_HIOS            = 0x6fffffff,
  SHT_LOPROC          = 0x70000000,
  SHT_HIPROC          = 0x7fffffff,
  SHT_LOUSER          = 0x80000000,
  SHT_HIUSER          = 0xffffffff
} elf_shtype_e;


// compression algorithms
typedef enum: uint32_t {
  ELFCOMPRESS_ZLIB     = 1,
  ELFCOMPRESS_LOOS     = 0x60000000,
  ELFCOMPRESS_HIOS     = 0x6fffffff,
  ELFCOMPRESS_LOPROC   = 0x70000000,
  ELFCOMPRESS_HIPROC   = 0x7fffffff
} elf_cmpalg_e;

// 32 bit entry format in section header table
typedef struct {
	Elf32_Word	  sh_name; // index in section header string table section referring to the beginning of a null-terminated string
	elf_shtype_e	sh_type; // type of the section, defines content and semantic
	Elf32_Word	  sh_flags; // flags for the section 
	Elf32_Addr	  sh_addr; // if section will be in process memory image, contains the start address, otherwise 0
	Elf32_Off	    sh_offset;
	Elf32_Word	  sh_size;
	Elf32_Word	  sh_link;
	Elf32_Word	  sh_info;
	Elf32_Word	  sh_addralign;
	Elf32_Word	  sh_entsize;
} __attribute__((packed)) Elf32_Shdr;

typedef struct {
	elf_cmpalg_e	ch_type; // compression alghorithm
	Elf32_Word	  ch_size; // size of uncompressed data
	Elf32_Word	  ch_addralign; // required alignment for uncompressed data
} __attribute__((packed)) Elf32_Chdr;


// 64 bit entry of section header table
typedef struct {
	Elf64_Word	  sh_name;
	elf_shtype_e  sh_type;
	Elf64_Xword	  sh_flags;
	Elf64_Addr	  sh_addr;
	Elf64_Off	    sh_offset;
	Elf64_Xword	  sh_size;
	Elf64_Word	  sh_link;
	Elf64_Word	  sh_info;
	Elf64_Xword	  sh_addralign;
	Elf64_Xword	  sh_entsize;
} __attribute__((packed)) Elf64_Shdr;

typedef struct {
	elf_cmpalg_e	ch_type;
	Elf64_Word	  ch_reserved;
	Elf64_Xword	  ch_size;
	Elf64_Xword	  ch_addralign;
} __attribute__((packed)) Elf64_Chdr;


#endif
