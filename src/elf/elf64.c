#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <elf/elf64.h>
#include <stdio.h>
#include <elf/elf_utils.h>
#include <error/error.h>
#include <errno.h>
#include <utility/utils.h>


#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)
#define __private static



elf_s *extract_sht64(elf_s *e_file) {
  Elf64_Ehdr *hdr       = (Elf64_Ehdr*)e_file->header;
  Elf64_Off  shoff      = hdr->e_shoff;
  Elf64_Half shentsize  = hdr->e_shentsize;
  e_file->sh_num = hdr->e_shnum;
  // no section headers
  if(!shoff && !e_file->sh_num) {
    e_file->sht = NULL;
    return e_file;
  }
  if(e_file->sh_num == SHN_UNDEF) {
    // read first section header to get actual section header number
    Elf64_Shdr s;
    fseek(e_file->fd, shoff, SEEK_SET);
    if(fread(&s, sizeof(Elf64_Shdr), 1, e_file->fd) < 1)
      goto err;
    e_file->sh_num = s.sh_size;
  }
  e_file->sht = malloc(sizeof(Elf64_Shdr) * e_file->sh_num);
  if(!e_file->sht)
    die("Memory allocation failed, try again!!");
  fseek(e_file->fd, shoff, SEEK_SET);
  if(fread(e_file->sht, shentsize, e_file->sh_num, e_file->fd) < e_file->sh_num) 
    goto err;
  return e_file;
err:
  fprintf(stderr, "Cannot read section table of %s(maybe corrupted): %s, skipping it...\n", e_file->name, strerror(errno));
  return NULL;
}




const Elf_Byte *get_from_strtb64(const elf_s *e_file, const Elf64_Shdr *s, size_t index) {
  // empty string table for that section
  if(!s->sh_size) {
    return (const Elf_Byte*)"NOSTRTB";
  }
  Elf_Byte *buff = malloc(sizeof(Elf_Byte) * BUFF_LEN + 1);
  if(!buff)
    die("Memory allocation failed: %s", strerror(errno));
  fseek(e_file->fd, s->sh_offset + (index * sizeof(Elf_Byte)), SEEK_SET);
  if(fread(buff, sizeof(Elf_Byte), BUFF_LEN, e_file->fd) < BUFF_LEN) {
    if(!feof(e_file->fd))
      die("Error while reading from file: %s", strerror(errno));
  }
  buff[BUFF_LEN] = 0;
  // needs to be freed after use
  return buff;
}


const elf_s *extract_symtbs64(elf_s *e_file) {
  // scan sht to find SYM sections, and populate symtbs arr
  Elf64_Sym **symtbs = (Elf64_Sym**)e_file->symtbs;
  symtbs = malloc(sizeof(Elf64_Sym*) * SYMTB_MAX);
  if(!symtbs)
    die("Memory allocation failed: %s", strerror(errno));
  size_t k = 0;
  for(size_t i = 0; i < e_file->sh_num; i++) {
    Elf64_Shdr *s = &e_file->sht[i];
    if(s->sh_type == SHT_SYMTAB || s->sh_type == SHT_DYNSYM) {
      // read string table
      symtbs[k] = malloc(s->sh_size);
      if(!symtbs[k])
        die("Memory allocation failed: %s", strerror(errno));
      fseek(e_file->fd, s->sh_offset * sizeof(Elf_Byte), SEEK_SET);
      if(fread(symtbs[k], s->sh_size, 1, e_file->fd) < 1) {
        fprintf(stderr, "Error while extracting sym tables from %s, skipping it...\n", e_file->name);
        return NULL;
      }
      k++;
    }
  }
  return e_file;
}



void  print_header_info64(const elf_s *e_file) {
    Elf64_Ehdr *hdr = (Elf64_Ehdr*)e_file->header;
    printf(HDR_FMT, "Class:", "ELF64");
    const char *data_str = hdr->e_ident.EI_DATA == ELFDATA2LSB ? "2's complement, little endian" : "2's complement, big endian";
    printf(HDR_FMT, "Data:", data_str);
    char buff[BUFF_LEN];
    sprintf(buff, "%d%s", hdr->e_ident.EI_VERSION, ( hdr->e_ident.EI_VERSION == EV_CURRENT ? " (current)" : ""));
    printf(HDR_FMT, "Version:", buff);
    printf(HDR_FMT, "OS/ABI:", elf_osabi_name(hdr->e_ident.EI_OSABI));
    sprintf(buff, "%d", hdr->e_ident.EI_ABIVERSION);
    printf(HDR_FMT, "ABI Version:", buff);
    printf(HDR_FMT, "Type:", elf_type_name(hdr->e_type));
    printf(HDR_FMT, "Machine:", elf_machine_name(hdr->e_machine)); 
    sprintf(buff, "0x%x", hdr->e_version);
    printf(HDR_FMT, "Version:", buff);
    printf(HDR_FMT, "Entry point address:", elf_entry_point(hdr->e_entry, buff));
    sprintf(buff, "%lu (%s)", hdr->e_phoff, hdr->e_phoff ? "bytes into file" : "no program headers");
    printf(HDR_FMT, "Start of program headers:", buff);
    sprintf(buff, "%lu (%s)", hdr->e_shoff, hdr->e_shoff ? "bytes into file" : "no section headers");
    printf(HDR_FMT, "Start of section headers:", buff);
    sprintf(buff, "0x%x", hdr->e_flags);
    printf(HDR_FMT, "Flags:", buff);
    sprintf(buff, "%hu (bytes)", hdr->e_ehsize);
    printf(HDR_FMT, "Size of this header:", buff);
    sprintf(buff, "%hu (bytes)", hdr->e_phentsize);
    printf(HDR_FMT, "Size of program headers:", buff);
    sprintf(buff, "%hu", hdr->e_phnum);
    printf(HDR_FMT, "Number of program headers:", buff);
    sprintf(buff, "%hu (bytes)", hdr->e_shentsize);
    printf(HDR_FMT, "Size of section headers:", buff);
    sprintf(buff, "%hu", hdr->e_shnum);
    printf(HDR_FMT, "Number of section headers:", buff);
    printf(HDR_FMT, "Section header string table index:", elf_shstrndx(hdr->e_shstrndx, buff));
}


__private void print_section(const elf_s *e_file, const Elf64_Shdr *s, size_t index) {
  // print table header
  if(!s) {
    char buff[32] = "[Nr]";
    center_string(buff, SHT_FIELD_WIDTH / 2);
    printf(SHT_FMT, buff, "Name", "Size", "Type", "EntSize", "Address", "Flags    Link    Info", "Offset", "Align");
    return;
  }
  // calculate section headers table string table index
  char fields[9][BUFF_LEN - 128];
  sprintf(fields[0], "[%zu]", index);
  center_string(fields[0], SHT_FIELD_WIDTH / 2);
  sht_name(fields[1], (char *)get_from_strtb64(e_file, &((const Elf64_Shdr*)e_file->sht)[elf_shtstrtb(e_file)], s->sh_name));
  sprintf(fields[2], "%.*lu", SHT_FIELD_WIDTH, s->sh_size);
  sprintf(fields[3], "%s", sht_type(s->sh_type));
  sprintf(fields[4], "%.*lu", SHT_FIELD_WIDTH, s->sh_entsize);
  sprintf(fields[5], "%.*lx", SHT_FIELD_WIDTH, s->sh_addr);
  unsigned int dim = (SHT_FIELD_WIDTH - 6) / 3 + 1;
  char flags[dim], link[dim], info[dim];
  sht_flags(flags, s->sh_flags);
  sprintf(link, "%d",  s->sh_link);
  sprintf(info, "%d",  s->sh_info);
  center_string(flags, dim);
  center_string(link, dim);
  center_string(info, dim);
  sprintf(fields[6], "%-*s   %-*s   %-*s", dim, flags, dim, link, dim, info);
  sprintf(fields[7], "%.*lu", SHT_FIELD_WIDTH/2, s->sh_offset);
  char buff[SHT_FIELD_WIDTH/2+1];
  sprintf(buff, "%lu", s->sh_addralign);
  center_string(buff, SHT_FIELD_WIDTH/2);
  sprintf(fields[8], "%-*s",  SHT_FIELD_WIDTH/2, buff); 
  printf(SHT_FMT, fields[0], fields[1], fields[2], fields[3], fields[4], fields[5], fields[6], fields[7], fields[8]);
}

void  print_sht64(const elf_s *e_file) {
  Elf64_Ehdr *hdr = e_file->header;
  Elf64_Shdr *sht = e_file->sht;
  printf("There are %lu sections headers, starting at offset 0x%lx\n", e_file->sh_num, hdr->e_shoff);
  puts("Section Headers:");
  print_section(e_file, NULL, 0);
  for(size_t i = 0; i < e_file->sh_num; i++)
    print_section((const elf_s*)e_file, &sht[i], i);
}



