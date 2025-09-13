#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <error/error.h>
#include <elf/elf32.h>
#include <stdio.h>
#include <elf/elf_utils.h>

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



void  print_sht32(const elf_s *e_file) {
  Elf32_Ehdr *hdr = e_file->header;
  //Elf32_Shdr *sht = e_file->sht;
  printf("There are %lu sections headers, starting at offset 0x%x\n", e_file->sh_num, hdr->e_shoff);
  puts("Section Headers:");
}


