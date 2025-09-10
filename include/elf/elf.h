#ifndef __ELF_H__
#define __ELF_H__

#include <stdlib.h>
#include "elf_header.h"
#include "elf_section.h"

#define Elf_Ehdr void

// ELF file struct
typedef struct {
  char            *name;
  Elf_Ehdr        *header;
  elf_class_e     type;
  void            *sections;
} elf_s;


// functions
elf_s *extract_header(elf_s *e_file);
void  print_header_info(const elf_s *e_file);
void  free_elf_list(elf_s **list, size_t len);

#endif
