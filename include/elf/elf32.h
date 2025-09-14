#ifndef __ELF32__
#define __ELF32__

#include <stdio.h>
// all elf32 functions

#include "elf_header.h"
#include "elf_section.h"
#include "elf_common.h"


elf_s *extract_sht32(elf_s *e_file, FILE *fd);
elf_s *extract_strtb32(elf_s *e_file, FILE *fd);


// option functions
void  print_header_info32(const elf_s *e_file);
void  print_sht32(const elf_s *e_file);


#endif
