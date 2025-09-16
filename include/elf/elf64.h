#ifndef __ELF64__
#define __ELF64__

#include <stdio.h>
// all elf64 functions

#include "elf_header.h"
#include "elf_section.h"
#include "elf_symtb.h"
#include "elf_common.h"


elf_s          *extract_sht64(elf_s *e_file);
const elf_s    *extract_symtbs64(elf_s *e_file);
const Elf_Byte *get_from_strtb64(const elf_s *e_file, const Elf64_Shdr *s, size_t index);


// option functions
void           print_header_info64(const elf_s *e_file);
void           print_sht64(const elf_s *e_file);
void           print_symtbs64(const elf_s *e_file);

#endif
