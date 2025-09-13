#ifndef __ELF_UTILS__
#define __ELF_UTILS__

#include <stdio.h>
#include "elf_header.h"


#define HDR_FMT   "%-40s%-30s\n"
#define SHT_FMT   "%-15s%-30s%-30s%-30s%-15s\n"
#define BUFF_LEN  256

// ELF header utilities
const char    *elf_osabi_name(elf_osabi_e osabi);
const char    *elf_type_name(elf_filetype_e filetype);
const char    *elf_machine_name(elf_machine_e machine);
char          *elf_entry_point(unsigned long long addr, char *buff);
char          *elf_shstrndx(unsigned short index, char *buff);
unsigned int  iself(const e_ident_s *ident);
int           read_hdr(int mode, elf_s *f, FILE *fd);



#endif
