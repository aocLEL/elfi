#ifndef __ELF_H__
#define __ELF_H__

#include <stdio.h>
#include "elf_common.h"


// functions
elf_s           *extract_header(elf_s *e_file, FILE *fd);
elf_s           *extract_sht(elf_s *e_file, FILE *fd);
elf_s           *extract_strtb(elf_s *e_file, FILE *fd);
const Elf_Byte  *get_from_strtb(const elf_s *e_file, size_t index);

void            print_header_info(const elf_s *e_file);
void            print_sht(const elf_s *e_file);
void            free_elf_list(elf_s **list, size_t len);

#endif
