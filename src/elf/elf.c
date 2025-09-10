#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <elf/elf.h>
#include <error/error.h>

#define __private static
#define HDR_FMT   "%-40s%-30s\n"
#define BUFF_LEN  128




__private const char *elf_osabi_name(elf_osabi_e osabi) {
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

__private const char *elf_type_name(elf_filetype_e filetype) {
  switch(filetype) {
    case ET_NONE:     return "NONE (No file type)";
    case ET_REL:      return "REL  (Relocatable file)";
    case ET_EXEC:     return "EXEC (Executable file)";
    case ET_DYN:      return "DYN  (Shared object file)";
    case ET_CORE:     return "CORE (Core file)";
    case ET_LOOS:     return "LOOS (Operating system specific)";
    case ET_HIOS:     return "HIOS (Operating system specific)";
    case ET_LOPROC:   return "LOPROC (Processor specific)";
    case ET_HIPROC:   return "HIPROC (Processor specific)";
    default:          return "Unrecognized type";
  }
}

__private const char *elf_machine_name(elf_machine_e machine) {
  switch(machine) {
    case EM_NONE:           return "NONE (No machine)";
    case EM_X86_64:         return "AMD x86-64 architecture";
    default:                return "Invalid machine";
  }
}

__private char *elf_entry_point(unsigned long long addr, char *buff) {
  if(!addr)
    sprintf(buff, "0x0 (No entry point)");
  else
    sprintf(buff, "0x%llx", addr);
  return buff;
}


__private char *elf_shstrndx(unsigned short index, char *buff) {
  if(index == SHN_UNDEF) 
    sprintf(buff, "UNDEF (No section header string table)");
  else if(index == SHN_XINDEX)
    sprintf(buff, "INDEX > 0xff00(LORESERVE), sh_link -> 0xfffff");
  else
    sprintf(buff, "%hu", index);
  // ricordarsi di sistemare il caso in cui bisogna andare a vedere nella prima entry della section headers table
  return buff;
}


// positive for 32, 0 for 64
__private int read_hdr(int mode, elf_s *f, FILE *fd) {
  size_t hsize = mode ? sizeof(Elf32_Ehdr) : sizeof(Elf64_Ehdr);
  f->header = malloc(hsize);
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
      if(!read_hdr(1, e_file, fd)) goto onerr;
      break;
    case ELFCLASS64:
      if(!read_hdr(0, e_file, fd)) goto onerr;
      break;
    default:
      fprintf(stderr, "File %s has invalid class, skipping...", e_file->name);
      return NULL;
  }
  fclose(fd);
  return e_file;
onerr:
    fclose(fd);
    return NULL;
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
}




void free_elf_list(elf_s **list, size_t len) {
  // per ogni elf_s dobbiamo rilasciare intanto il suo header, poi dopo lo modifichi man mano che implementi il resto dell'elf
  for(size_t i = 0; i < len && list[i]; i++) {
    free(list[i]->header);
    free(list[i]);
  }
  free(list);
}
