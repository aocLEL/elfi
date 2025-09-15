#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <elf/elf.h>
#include <elf/elf_utils.h>
#include <utility/utils.h>
#include <elf/elf32.h>
#include <elf/elf64.h>
#include <error/error.h>


#define __private static





const elf_s *extract_header(elf_s *e_file) {
  e_ident_s ident; 
  if(fread(&ident, sizeof(e_ident_s), 1, e_file->fd) < 1) {
    fprintf(stderr, "Cannot read file %s: %s\n", e_file->name, strerror(errno));
    return NULL;
  }
  if(!iself(&ident)) {
    fprintf(stderr, "Invalid magic detected!! \"%s\" is not an ELF binary\n", e_file->name);
    return NULL;
  }
  // check ELF format
  switch(ident.EI_CLASS) {
    case ELFCLASS32:
      if(!read_hdr(1, e_file)) goto onerr;
      break;
    case ELFCLASS64:
      if(!read_hdr(0, e_file)) goto onerr;
      break;
    default:
      fprintf(stderr, "File %s has invalid class, skipping...", e_file->name);
      return NULL;
  }
  return (const elf_s*)e_file;
onerr:
    return NULL;
}


const elf_s *extract_sht(elf_s *e_file) {
  if(e_file->type == ELFCLASS64) 
    return extract_sht64(e_file);
  else
    return extract_sht32(e_file);
}


const elf_s *extract_symtbs(elf_s *e_file) {
  if(e_file->type == ELFCLASS64)
    return extract_symtbs64(e_file);
  else 
    return extract_symtbs32(e_file);
}

// return a reference
const Elf_Byte  *get_from_strtb(const elf_s *e_file, const Elf_Shdr *s, size_t index) {
  if(e_file->type == ELFCLASS64)
    return get_from_strtb64(e_file, (const Elf64_Shdr*)s, index);
  return get_from_strtb32(e_file, (const Elf32_Shdr*)s, index);
}


void print_header_info(const elf_s *e_file) {
  // cast header pointer to correct type
  // Magic ident
  puts("ELF header:");
  printf("Magic:\t");
  Elf_Byte *raw_ident = (Elf_Byte *)e_file->header;
  for(size_t i = 0; i < EI_NIDENT; i++) 
    printf("%02X ", raw_ident[i]);
  puts("");
  if(e_file->type == ELFCLASS64)
    print_header_info64(e_file);
  else
    print_header_info32(e_file);
}


void print_sht(const elf_s *e_file) {
  if(e_file->type == ELFCLASS64)
    print_sht64(e_file);
  else
    print_sht32(e_file);
}


void free_elf_list(elf_s **list, size_t len) {
  // per ogni elf_s dobbiamo rilasciare intanto il suo header, poi dopo lo modifichi man mano che implementi il resto dell'elf
  for(size_t i = 0; i < len && list[i]; i++) {
    fclose(list[i]->fd); // closing file descriptor
    free(list[i]->header);
    free(list[i]->sht);
    for(size_t k = 0; k < SYMTB_MAX && list[i]->symtbs[k]; k++)
      free(list[i]->symtbs[k]);
    free(list[i]->symtbs);
    free(list[i]);
  }
  free(list);
}



