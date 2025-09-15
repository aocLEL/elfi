#ifndef __ELF_H__
#define __ELF_H__

#include <stdio.h>
#include "elf_common.h"


// functions
const elf_s           *extract_header(elf_s *e_file);
const elf_s           *extract_sht(elf_s *e_file);
const elf_s           *extract_symtbs(elf_s *e_file);
// read a value at specific offset of string table pointed by s section(this could also be a symbol table)
const Elf_Byte        *get_from_strtb(const elf_s *e_file, const Elf_Shdr *s, size_t index);

void                  print_header_info(const elf_s *e_file);
void                  print_sht(const elf_s *e_file);
void                  free_elf_list(elf_s **list, size_t len);

#endif
