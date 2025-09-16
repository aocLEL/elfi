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



elf_s *extract_sht32(elf_s *e_file) {
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
    fseek(e_file->fd, shoff, SEEK_SET);
    if(fread(&s, sizeof(Elf32_Shdr), 1, e_file->fd) < 1)
      goto err;
    e_file->sh_num = s.sh_size;
  }
  e_file->sht = malloc(sizeof(Elf32_Shdr) * e_file->sh_num);
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



const elf_s *extract_symtbs32(elf_s *e_file) {
  // scan sht to find SYM sections, and populate symtbs arr
  elf_symtb_s *symtbs = malloc(sizeof(elf_symtb_s) * SYMTB_MAX);
  memset(symtbs, 0, sizeof(elf_symtb_s) * SYMTB_MAX);
  if(!symtbs)
    die("Memory allocation failed: %s", strerror(errno));
  size_t k = 0;
  for(size_t i = 0; i < e_file->sh_num; i++) {
    Elf32_Shdr *s = &((Elf32_Shdr*)e_file->sht)[i];
    if(s->sh_type == SHT_SYMTAB || s->sh_type == SHT_DYNSYM) {
      // read string table
      // symbols pointer
      symtbs[k].symtb_sidx = i;
      symtbs[k].entries = s->sh_size / sizeof(Elf32_Sym);
      Elf32_Sym *symbols = malloc(s->sh_size);
      if(!symbols)
        die("Memory allocation failed: %s", strerror(errno));
      fseek(e_file->fd, s->sh_offset * sizeof(Elf_Byte), SEEK_SET);
      if(fread(symbols, s->sh_size, 1, e_file->fd) < 1) {
        fprintf(stderr, "Error while extracting sym tables from %s, skipping it...\n", e_file->name);
        return NULL;
      }
      symtbs[k].symbols = symbols;
      k++;
    }
  }
  e_file->symtbs = symtbs;
  return e_file;
}




const Elf_Byte *get_from_strtb32(const elf_s *e_file, const Elf32_Shdr *s, size_t index) {
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
  trunc_name(fields[1], (char *)get_from_strtb32(e_file, &((const Elf32_Shdr*)e_file->sht)[elf_shtstrtb(e_file)], s->sh_name), SHT_FIELD_WIDTH - 5);
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


__private void print_symbol(const elf_s *e_file, const Elf32_Sym *sym, size_t symtb_sidx, size_t index) {
 Elf32_Shdr *sht = e_file->sht;
 char fields[8][BUFF_LEN];
 // Num
 sprintf(fields[0], "%*zu:", SYM_FIELD_WIDTH - 6, index);
 // Value
 sprintf(fields[1], "%.*x", SYM_FIELD_WIDTH - 6, sym->st_value);
 // Size
 sprintf(fields[2], "%*u", SYM_FIELD_WIDTH - 17, sym->st_size);
 // type
 sprintf(fields[3], "%*s", SYM_FIELD_WIDTH - 14, elf_symtype(ELF32_ST_TYPE(sym->st_info)));
 sprintf(fields[4], "%*s", SYM_FIELD_WIDTH - 15, elf_symbind(ELF32_ST_BIND(sym->st_info)));
 sprintf(fields[5], "%*s", SYM_FIELD_WIDTH - 13, elf_symvis(ELF32_ST_VISIBILITY(sym->st_other)));
 // symbol section index
 switch(sym->st_shndx) {
   case SHN_ABS:
     sprintf(fields[6], "%*s", SYM_FIELD_WIDTH - 18, "ABS");
     break;
   case SHN_UNDEF:
     sprintf(fields[6], "%*s", SYM_FIELD_WIDTH - 18, "UND");
     break;
   case SHN_XINDEX:
     for(size_t i = 0; i < e_file->sh_num; i++) {
       if(sht[i].sh_type == SHT_SYMTAB_SHNDX && sht[i].sh_link == symtb_sidx) {
          fseek(e_file->fd, 0, SEEK_SET);
          Elf32_Word *ext_indexes = malloc(sht[i].sh_size);
          if(!ext_indexes)
            die("Memory allocation error: %s", strerror(errno));
          if(fread(ext_indexes, sht[i].sh_size, 1, e_file->fd) < 1) {
            fprintf(stderr, "Cannot find section of symbol %x, UNDEF is used!!\n", sym->st_value);
            sprintf(fields[6], "%*s", SYM_FIELD_WIDTH - 18, "***"); 
            break;
          }
          sprintf(fields[6], "%*u", SYM_FIELD_WIDTH - 18, ext_indexes[index]); 
          break;
       }
     }
     break;
   default:
     sprintf(fields[6], "%*hu", SYM_FIELD_WIDTH - 18, sym->st_shndx); 
 }
 char name[BUFF_LEN];
 size_t symstrtbi  =  sht[symtb_sidx].sh_link; 
 trunc_name(name, (char *)get_from_strtb32(e_file, &((const Elf32_Shdr*)e_file->sht)[symstrtbi], sym->st_name), SYM_FIELD_WIDTH - 5);
 sprintf(fields[7], "%*s", SYM_FIELD_WIDTH, name);
 printf(SYM_FMT, fields[0], fields[1], fields[2], fields[3], fields[4], fields[5], fields[6], fields[7]);
}

void print_symtbs32(const elf_s *e_file) {
  Elf32_Shdr *sht = e_file->sht;
  for(size_t i = 0; i < SYMTB_MAX && e_file->symtbs[i].entries; i++) {
    size_t symtb_sidx = e_file->symtbs[i].symtb_sidx;
    // get section name from section header table string table
    unsigned char *symtb_name = (unsigned char *)get_from_strtb32(e_file, &((const Elf32_Shdr*)e_file->sht)[elf_shtstrtb(e_file)], sht[symtb_sidx].sh_name);
    size_t entries = e_file->symtbs[i].entries;
    printf("Symbol table \'%s\' contains %lu entries:\n", symtb_name, entries);
    printf(SYM_FMT, "Num:", " Value", "Size", "Type", "Bind", "Vis", "Ndx", "Name");
    for(size_t j = 0; j < entries; j++)
      print_symbol(e_file, &((Elf32_Sym*)e_file->symtbs[i].symbols)[j], symtb_sidx, j);
    printf("\n\n");
    free(symtb_name);
  }
}
