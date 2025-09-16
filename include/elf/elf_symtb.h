#ifndef __ELF_SYMTB__
#define __ELF_SYMTB__

#include "elf_common.h"

#define SYMTB_MAX          10
#define ELF32_ST_BIND(i)   ((i)>>4)
#define ELF32_ST_TYPE(i)   ((i)&0xf)
#define ELF32_ST_INFO(b,t) (((b)<<4)+((t)&0xf))

#define ELF64_ST_BIND(i)   ((i)>>4)
#define ELF64_ST_TYPE(i)   ((i)&0xf)
#define ELF64_ST_INFO(b,t) (((b)<<4)+((t)&0xf))


#define ELF32_ST_VISIBILITY(o) ((o)&0x3)
#define ELF64_ST_VISIBILITY(o) ((o)&0x3)

typedef enum: Elf_Byte {
  STB_LOCAL,
  STB_GLOBAL,
  STB_WEAK,
  STB_LOOS = 10,
  STB_HIOS = 12,
  STB_LOPROC,
  STB_HIPROC = 15
} elf_sym_bind_e;

typedef enum: Elf_Byte {
  STT_NOTYPE,
  STT_OBJECT, // var, arr
  STT_FUNC,
  STT_SECTION,// primarily for relocation
  STT_FILE, // associated source file name
  STT_COMMON,
  STT_TLS,
  STT_LOOS = 10,
  STT_HIOS = 12,
  STT_LOPROC,
  STT_HIPROC  = 15
} elf_sym_type_e;

typedef enum: Elf_Byte {
  STV_DEFAULT,
  STV_INTERNAL,
  STV_HIDDEN,
  STV_PROTECTED
} elf_sym_vis_e;


// symbol table entry
typedef struct {
	Elf32_Word	st_name;// index pointing to symbol char representation in symbol string table
	Elf32_Addr	st_value; // symbol's value
	Elf32_Word	st_size; // size of value in bytes, 0 if unknown
	Elf_Byte	  st_info; // symbol's type and binding attributes
	Elf_Byte	  st_other; // symbol's visibility
	Elf32_Half	st_shndx; // section header table index of symbol's section, if too big, it contains SHN_XINDEX and actual value is in section SHT_SYMTAB_SHNDX
} __attribute__((packed)) Elf32_Sym;

typedef struct {
	Elf64_Word	st_name;
	Elf_Byte	  st_info;
	Elf_Byte	  st_other;
	Elf64_Half	st_shndx;
	Elf64_Addr	st_value;
	Elf64_Xword	st_size;
} __attribute__((packed)) Elf64_Sym;

#endif
