#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>
#include <error/error.h>
#include <elf/elf32.h>
#include <utility/utils.h>
#include <stdio.h>
#include <elf/elf_utils.h>

#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)

extern const Elf_Byte  *get_from_strtb(const elf_s *e_file, size_t index);


elf_s *extract_sht32(elf_s *e_file, FILE *fd) {
  Elf32_Ehdr *hdr       = (Elf32_Ehdr*)e_file->header;
  Elf32_Off  shoff      = hdr->e_shoff;
  Elf32_Half shentsize  = hdr->e_shentsize;
  e_file->sh_num = hdr->e_shnum;
  // no section headers
  if(!shoff && !e_file->sh_num) {
    e_file->sht = NULL;
    return e_file;
  }
  if(e_file->sh_num == SHN_UNDEF) {
    // read first section header to get actual section header number
    Elf32_Shdr s;
    fseek(fd, shoff, SEEK_SET);
    if(fread(&s, sizeof(Elf32_Shdr), 1, fd) < 1)
      goto err;
    e_file->sh_num = s.sh_size;
  }
  e_file->sht = malloc(sizeof(Elf32_Shdr) * e_file->sh_num);
  if(!e_file->sht)
    die("Memory allocation failed, try again!!");
  fseek(fd, shoff, SEEK_SET);
  if(fread(e_file->sht, shentsize, e_file->sh_num, fd) < e_file->sh_num) 
    goto err;
  return e_file;
err:
  fprintf(stderr, "Cannot read section table of %s(maybe corrupted): %s, skipping it...\n", e_file->name, strerror(errno));
  return NULL;
}



elf_s *extract_strtb32(elf_s *e_file, FILE *fd) {
  // get string table header index in section headers table
  const Elf32_Ehdr *hdr = e_file->header;
  const Elf32_Shdr *sht = e_file->sht;
  // if is bigger than loreserve, take it from section 0 sh_link field
  size_t strtbi = hdr->e_shstrndx >= SHN_LORESERVE ? sht[0].sh_link : hdr->e_shstrndx; 
  // read entire string table on memory
  if(!sht[strtbi].sh_size) {
    fprintf(stderr, "File %s has no string table, indexes will be used!!", e_file->name);
    e_file->strtb = NULL;
    return NULL;
  }
  size_t page_align_diff = sht[strtbi].sh_offset - ((size_t)(sht[strtbi].sh_offset / PAGE_SIZE) * PAGE_SIZE);
  e_file->strtb = mmap(NULL, (sht[strtbi].sh_size + page_align_diff) * sizeof(Elf_Byte), PROT_READ, 0, fileno(fd), sht[strtbi].sh_offset - page_align_diff);
  e_file->strtb = e_file->strtb + page_align_diff;
  if(e_file->strtb == MAP_FAILED)
    die("!!!STRING TABLE MAPPING ERROR: %s", strerror(errno));
  return e_file; 
}


void  print_header_info32(const elf_s *e_file) {
    // 32 bit header
    Elf32_Ehdr *hdr = (Elf32_Ehdr*)e_file->header;
    printf(HDR_FMT, "Class:", "ELF32");
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
    sprintf(buff, "%u (%s)", hdr->e_phoff, hdr->e_phoff ? "bytes into file" : "no program headers");
    printf(HDR_FMT, "Start of program headers:", buff);
    sprintf(buff, "%u (%s)", hdr->e_shoff, hdr->e_shoff ? "bytes into file" : "no section headers");
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





__private void print_section(const elf_s *e_file, const Elf32_Shdr *s, size_t index) {
  // print table header
  if(!s) {
    char buff[32] = "[Nr]";
    center_string(buff, SHT_FIELD_WIDTH / 2);
    printf(SHT_FMT, buff, "Name", "Size", "Type", "EntSize", "Address", "Flags    Link    Info", "Offset", "Align");
    return;
  }  
  char fields[9][BUFF_LEN - 128];
  sprintf(fields[0], "[%zu]", index);
  center_string(fields[0], SHT_FIELD_WIDTH / 2);
  sht_name(fields[1], (const char *)get_from_strtb(e_file, s->sh_name));
  sprintf(fields[2], "%.*u", SHT_FIELD_WIDTH, s->sh_size);
  sprintf(fields[3], "%s", sht_type(s->sh_type));
  sprintf(fields[4], "%.*u", SHT_FIELD_WIDTH, s->sh_entsize);
  sprintf(fields[5], "%.*x", SHT_FIELD_WIDTH, s->sh_addr);
  unsigned int dim = (SHT_FIELD_WIDTH - 6) / 3 + 1;
  char flags[dim], link[dim], info[dim];
  sht_flags(flags, s->sh_flags);
  sprintf(link, "%d",  s->sh_link);
  sprintf(info, "%d",  s->sh_info);
  center_string(flags, dim);
  center_string(link, dim);
  center_string(info, dim);
  sprintf(fields[6], "%-*s   %-*s   %-*s", dim, flags, dim, link, dim, info);
  sprintf(fields[7], "%.*u", SHT_FIELD_WIDTH/2, s->sh_offset);
  char buff[SHT_FIELD_WIDTH/2+1];
  sprintf(buff, "%u", s->sh_addralign);
  center_string(buff, SHT_FIELD_WIDTH/2);
  sprintf(fields[8], "%-*s",  SHT_FIELD_WIDTH/2, buff); 
  printf(SHT_FMT, fields[0], fields[1], fields[2], fields[3], fields[4], fields[5], fields[6], fields[7], fields[8]);
}

void  print_sht32(const elf_s *e_file) {
  Elf32_Ehdr *hdr = e_file->header;
  Elf32_Shdr *sht = e_file->sht;
  printf("There are %lu sections headers, starting at offset 0x%x\n", e_file->sh_num, hdr->e_shoff);
  puts("Section Headers:");
  print_section(e_file, NULL, 0);
  for(size_t i = 0; i < e_file->sh_num; i++)
    print_section((const elf_s*)e_file, &sht[i], i);
}

