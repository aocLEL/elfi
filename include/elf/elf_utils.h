#ifndef __ELF_UTILS__
#define __ELF_UTILS__

#include <stdio.h>
#include "elf_header.h"
#include "elf_section.h"
#include "elf_symtb.h"


#define HDR_FMT         "%-40s%-30s\n"
#define SHT_FMT         "%-15s%-20s%-20s\t%-20s\t%-20s\t%-20s\t%-25s\t%-10s\t%-10s\n"
#define SYM_FMT         "%15s %-16s  %4s %-7s %-6s %-8s %-3s %-21s\n"
#define SHT_FIELD_WIDTH 20
#define SYM_FIELD_WIDTH 21
#define BUFF_LEN        256

// ELF header utilities
const char    *elf_osabi_name(elf_osabi_e osabi);
const char    *elf_type_name(elf_filetype_e filetype);
const char    *elf_machine_name(elf_machine_e machine);
char          *elf_entry_point(unsigned long long addr, char *buff);
char          *elf_shstrndx(unsigned short index, char *buff);
unsigned int  iself(const e_ident_s *ident);
int           read_hdr(int mode, elf_s *f);
// ELF section header utilities
size_t        elf_shtstrtb(const elf_s *e_file);
const char    *sht_type(elf_shtype_e type);
const char    *sht_flags(char *buff, size_t flags);
// ELF symtab utilities
const char    *elf_symtype(elf_sym_type_e type);
const char    *elf_symbind(elf_sym_bind_e bind);
const char    *elf_symvis(elf_sym_vis_e vis);
// common
const char    *trunc_name(char *buff, char *name, unsigned int width);





#endif
