#ifndef __ELF32__
#define __ELF32__

#include <stdio.h>
// all elf32 functions

#include "elf_header.h"
#include "elf_section.h"
#include "elf_symtb.h"
#include "elf_common.h"


elf_s          *extract_sht32(elf_s *e_file);
const elf_s    *extract_symtbs32(elf_s *e_file);
const Elf_Byte *get_from_strtb32(const elf_s *e_file, const Elf32_Shdr *s, size_t index);


// option functions
void           print_header_info32(const elf_s *e_file);
void           print_sht32(const elf_s *e_file);
void           print_symtbs32(const elf_s *e_file);


#endif
