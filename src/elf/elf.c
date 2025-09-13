#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <elf/elf.h>
#include <elf/elf_utils.h>
#include <elf/elf32.h>
#include <elf/elf64.h>
#include <error/error.h>


#define __private static





elf_s *extract_header(elf_s *e_file, FILE *fd) {
  e_ident_s ident; 
  if(fread(&ident, sizeof(e_ident_s), 1, fd) < 1) {
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
      if(!read_hdr(1, e_file, fd)) goto onerr;
      break;
    case ELFCLASS64:
      if(!read_hdr(0, e_file, fd)) goto onerr;
      break;
    default:
      fprintf(stderr, "File %s has invalid class, skipping...", e_file->name);
      return NULL;
  }
  return e_file;
onerr:
    return NULL;
}


elf_s *extract_sht(elf_s *e_file, FILE *fd) {
  // take shoff and shentsize from the header
  // check shnum, if >= loreserve than read only the first entry and take the total section number, otherwise read all the table
  // continue here, test new structure before
  elf_s *retval;
  if(e_file->type == ELFCLASS64) 
    retval = extract_sht64(e_file, fd);
  else
    retval = extract_sht32(e_file, fd);
  return retval;
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
    free(list[i]->header);
    free(list[i]->sht);
    free(list[i]);
  }
  free(list);
}



