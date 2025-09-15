#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <error/error.h>
#include <elf/elf_common.h>
#include <elf/elf_utils.h>


const char *elf_osabi_name(elf_osabi_e osabi) {
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

const char *elf_type_name(elf_filetype_e filetype) {
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

const char *elf_machine_name(elf_machine_e machine) {
  switch(machine) {
    case EM_NONE:           return "NONE (No machine)";
    case EM_X86_64:         return "AMD x86-64 architecture";
    default:                return "Invalid machine";
  }
}

char *elf_entry_point(unsigned long long addr, char *buff) {
  if(!addr)
    sprintf(buff, "0x0 (No entry point)");
  else
    sprintf(buff, "0x%llx", addr);
  return buff;
}


char *elf_shstrndx(unsigned short index, char *buff) {
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
int read_hdr(int mode, elf_s *f) {
  size_t hsize = mode ? sizeof(Elf32_Ehdr) : sizeof(Elf64_Ehdr);
  f->header = malloc(hsize);
  if(!f->header) {
    die("Memory allocation failed, try again!!");
  }
  f->type = mode ? ELFCLASS32 : ELFCLASS64;
  fseek(f->fd, 0, SEEK_SET);
  if(fread(f->header, hsize, 1, f->fd) < 1){
    fprintf(stderr, "Cannot read file %s: %s\n", f->name, strerror(errno));
    return 0;
  }
  return 1; 
}

unsigned int iself(const e_ident_s *ident) {
  return (ident->EI_MAG0 == ELFMAG0 && ident->EI_MAG1 == ELFMAG1 && ident->EI_MAG2 == ELFMAG2 && ident->EI_MAG3 == ELFMAG3);
}





const char *sht_name(char *buff, char *name) {
  if(strlen(name) > SHT_FIELD_WIDTH)
    sprintf(buff, "%.*s[...]", SHT_FIELD_WIDTH - 5, name);
  else
    sprintf(buff, "%s", name);
  free(name);
  return buff;
}

size_t elf_shtstrtb(const elf_s *e_file) {
  if(e_file->type == ELFCLASS64) {
    Elf64_Ehdr *hdr = e_file->header;
    Elf64_Shdr *sht = e_file->sht;
    return hdr->e_shstrndx >= SHN_LORESERVE ? sht[0].sh_link : hdr->e_shstrndx; 
  }
  Elf32_Ehdr *hdr = e_file->header;
  Elf32_Shdr *sht = e_file->sht;
  return hdr->e_shstrndx >= SHN_LORESERVE ? sht[0].sh_link : hdr->e_shstrndx; 

}




const char *sht_type(elf_shtype_e type) {
  switch(type) {
        case SHT_NULL:
            return "NULL";
        case SHT_PROGBITS:
            return "PROGBITS";
        case SHT_SYMTAB:
            return "SYMTAB";
        case SHT_STRTAB:
            return "STRTAB";
        case SHT_RELA:
            return "RELA";
        case SHT_HASH:
            return "HASH";
        case SHT_DYNAMIC:
            return "DYNAMIC";
        case SHT_NOTE:
            return "NOTE";
        case SHT_NOBITS:
            return "NOBITS";
        case SHT_REL:
            return "REL";
        case SHT_SHLIB:
            return "SHLIB";
        case SHT_DYNSYM:
            return "DYNSYM";
        case SHT_INIT_ARRAY:
            return "INIT_ARRAY";
        case SHT_FINI_ARRAY:
            return "FINI_ARRAY";
        case SHT_PREINIT_ARRAY:
            return "PREINIT_ARRAY";
        case SHT_GROUP:
            return "GROUP";
        case SHT_SYMTAB_SHNDX:
            return "SYMTAB_SHNDX";
        case SHT_LOOS:
            return "LOOS";
        case SHT_HIOS:
            return "HIOS";
        case SHT_LOPROC:
            return "LOPROC";
        case SHT_HIPROC:
            return "HIPROC";
        case SHT_LOUSER:
            return "LOUSER";
        case SHT_HIUSER:
            return "HIUSER";
        default:
            return "UNKNOWN";
  }
}


const char *sht_flags(char *buff, size_t flags) {
  if(flags & SHF_WRITE)               *buff++ = 'W';
  if(flags & SHF_ALLOC)               *buff++ = 'A';
  if(flags & SHF_EXECINSTR)           *buff++ = 'X';
  if(flags & SHF_MERGE)               *buff++ = 'M';
  if(flags & SHF_STRINGS)             *buff++ = 'S';
  if(flags & SHF_INFO_LINK)           *buff++ = 'I';
  if(flags & SHF_LINK_ORDER)          *buff++ = 'L';
  if(flags & SHF_OS_NONCONFORMING)    *buff++ = 'O';
  if(flags & SHF_GROUP)               *buff++ = 'G';
  if(flags & SHF_TLS)                 *buff++ = 'T';
  if(flags & SHF_COMPRESSED)          *buff++ = 'C';
  if(flags & SHF_MASKOS)              *buff++ = 'o';
  if(flags & SHF_MASKPROC)            *buff++ = 'p';
  *buff = 0;
  return buff;
}


