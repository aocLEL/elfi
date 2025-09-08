#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <elf/elf.h>
#include <error/error.h>

#define __private static
#define HDR_FMT   "%-20s%-30s\n"
#define BUFF_LEN  128




__private const char *osabi_name(elf_osabi_e osabi) {
  switch (osabi) {
        case ELFOSABI_NONE:     return "Unix - System V ABI";
        case ELFOSABI_HPUX:     return "Hewlett-Packard HP-UX";
        case ELFOSABI_NETBSD:   return "NetBSD";
        case ELFOSABI_LINUX:    return "Linux (alias for GNU)";
        case ELFOSABI_SOLARIS:  return "Sun Solaris";
        case ELFOSABI_AIX:      return "AIX";
        case ELFOSABI_IRIX:     return "IRIX";
        case ELFOSABI_FREEBSD:  return "FreeBSD";
        case ELFOSABI_TRU64:    return "Compaq TRU64 UNIX";
        case ELFOSABI_MODESTO:  return "Novell Modesto";
        case ELFOSABI_OPENBSD:  return "OpenBSD";
        case ELFOSABI_OPENVMS:  return "OpenVMS";
        case ELFOSABI_NSK:      return "Hewlett-Packard Non-Stop Kernel";
        case ELFOSABI_AROS:     return "Amiga Research OS";
        case ELFOSABI_FENIXOS:  return "FenixOS scalable multi-core OS";
        case ELFOSABI_CLOUDABI: return "Nuxi CloudABI";
        case ELFOSABI_OPENVOS:  return "Stratus Technologies OpenVOS";
        default:                return "Unknown OSABI";
    }
}



// positive for 32, 0 for 64
__private int read_hdr(int mode, elf_s *f, FILE *fd) {
  size_t hsize = mode ? sizeof(Elf32_Ehdr) : sizeof(Elf64_Ehdr);
  f->header = malloc(sizeof(hsize));
  if(!f->header) {
    die("Memory allocation failed, try again!!");
  }
  f->type = mode ? ELFCLASS32 : ELFCLASS64;
  fseek(fd, 0, SEEK_SET);
  if(fread(f->header, hsize, 1, fd) < 1){
    fprintf(stderr, "Cannot read file %s: %s\n", f->name, strerror(errno));
    return 0;
  }
  return 1; 
}

__private unsigned int iself(const e_ident_s *ident) {
  return (ident->EI_MAG0 == ELFMAG0 && ident->EI_MAG1 == ELFMAG1 && ident->EI_MAG2 == ELFMAG2 && ident->EI_MAG3 == ELFMAG3);
}


elf_s *extract_header(elf_s *e_file) {
  e_ident_s ident;
  FILE *fd = fopen(e_file->name, "r");
  if(!fd) {
    fprintf(stderr, "Cannot open file %s: %s\n", e_file->name, strerror(errno));
    return NULL;
  }
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
      if(!read_hdr(1, e_file, fd)) return NULL;
      break;
    case ELFCLASS64:
      if(!read_hdr(0, e_file, fd)) return NULL;
      break;
    default:
      break;
  }
  return e_file;
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
  if(e_file->type == ELFCLASS64) {
    Elf64_Ehdr *hdr = (Elf64_Ehdr*)e_file->header;
    printf(HDR_FMT, "Class:", "ELF64");
    const char *data_str = hdr->e_ident.EI_DATA == ELFDATA2LSB ? "2's complement, little endian" : "2's complement, big endian";
    printf(HDR_FMT, "Data:", data_str);
    char buff[BUFF_LEN];
    sprintf(buff, "%d%s", hdr->e_ident.EI_VERSION, ( hdr->e_ident.EI_VERSION == EV_CURRENT ? " (current)" : ""));
    printf(HDR_FMT, "Version:", buff);
    printf(HDR_FMT, "OS/ABI", osabi_name(hdr->e_ident.EI_OSABI));
    sprintf(buff, "%d", hdr->e_ident.EI_ABIVERSION);
    printf(HDR_FMT, "ABI Version:", buff);
  }
}
