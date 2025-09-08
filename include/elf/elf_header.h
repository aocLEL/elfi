#ifndef __ELF_HEADER_H__
#define __ELF_HEADER_H__

#include <stdint.h>
#include "elf_common.h"

// FIELD TYPES ----------

// ELF costants
#define EI_NIDENT     16
#define ELFMAG0       0x7f
#define ELFMAG1       0x45
#define ELFMAG2       0x4c
#define ELFMAG3       0x46


// elf class
typedef enum : Elf_Byte {
  ELFCLASSNONE,
  ELFCLASS32,
  ELFCLASS64
} elf_class_e;

// elf data
typedef enum : Elf_Byte {
  ELFDATANONE,
  ELFDATA2LSB,
  ELFDATA2MSB
} elf_data_e;

// elf version
typedef enum : Elf_Byte {
  EV_NONE,
  EV_CURRENT = 0x1
} elf_version_e;

// elf osabi extensions
typedef enum : uint8_t {
    ELFOSABI_NONE        = 0x00,  /* No extensions or unspecified */
    ELFOSABI_HPUX        = 0x01,  /* Hewlett-Packard HP-UX */
    ELFOSABI_NETBSD      = 0x02,  /* NetBSD */
    ELFOSABI_GNU         = 0x03,  /* GNU */
    ELFOSABI_LINUX       = 0x03,  /* Linux (alias for ELFOSABI_GNU) */
    ELFOSABI_SOLARIS     = 0x06,  /* Sun Solaris */
    ELFOSABI_AIX         = 0x07,  /* AIX */
    ELFOSABI_IRIX        = 0x08,  /* IRIX */
    ELFOSABI_FREEBSD     = 0x09,  /* FreeBSD */
    ELFOSABI_TRU64       = 0x0A,  /* Compaq TRU64 UNIX */
    ELFOSABI_MODESTO     = 0x0B,  /* Novell Modesto */
    ELFOSABI_OPENBSD     = 0x0C,  /* OpenBSD */
    ELFOSABI_OPENVMS     = 0x0D,  /* OpenVMS */
    ELFOSABI_NSK         = 0x0E,  /* Hewlett-Packard Non-Stop Kernel */
    ELFOSABI_AROS        = 0x0F,  /* Amiga Research OS */
    ELFOSABI_FENIXOS     = 0x10,  /* FenixOS scalable multi-core OS */
    ELFOSABI_CLOUDABI    = 0x11,  /* Nuxi CloudABI */
    ELFOSABI_OPENVOS     = 0x12   /* Stratus Technologies OpenVOS */
    // 0x4 - 0x100 reserved for future
} elf_osabi_e;


// elf file types
typedef enum : uint16_t {
  ET_NONE,
  ET_REL,
  ET_EXEC,
  ET_DYN,
  ET_CORE,
  ET_LOOS   = 0xfe00,
  ET_HIOS   = 0xfeff,
  ET_LOPROC = 0xff00,
  ET_HIPROC = 0xffff
} elf_filetype_e;


// elf machine type
typedef enum : uint16_t {
    EM_NONE              = 0x0,
    EM_M32               = 0x1,
    EM_SPARC             = 0x2,
    EM_386               = 0x3,
    EM_68K               = 0x4,
    EM_88K               = 0x5,
    EM_IAMCU             = 0x6,
    EM_860               = 0x7,
    EM_MIPS              = 0x8,
    EM_S370              = 0x9,
    EM_MIPS_RS3_LE       = 0xA,
    EM_PARISC            = 0xF,
    EM_VPP500            = 0x11,
    EM_SPARC32PLUS       = 0x12,
    EM_960               = 0x13,
    EM_PPC               = 0x14,
    EM_PPC64             = 0x15,
    EM_S390              = 0x16,
    EM_SPU               = 0x17,
    EM_V800              = 0x24,
    EM_FR20              = 0x25,
    EM_RH32              = 0x26,
    EM_RCE               = 0x27,
    EM_ARM               = 0x28,
    EM_ALPHA             = 0x29,
    EM_SH                = 0x2A,
    EM_SPARCV9           = 0x2B,
    EM_TRICORE           = 0x2C,
    EM_ARC               = 0x2D,
    EM_H8_300            = 0x2E,
    EM_H8_300H           = 0x2F,
    EM_H8S               = 0x30,
    EM_H8_500            = 0x31,
    EM_IA_64             = 0x32,
    EM_MIPS_X            = 0x33,
    EM_COLDFIRE          = 0x34,
    EM_68HC12            = 0x35,
    EM_MMA               = 0x36,
    EM_PCP               = 0x37,
    EM_NCPU              = 0x38,
    EM_NDR1              = 0x39,
    EM_STARCORE          = 0x3A,
    EM_ME16              = 0x3B,
    EM_ST100             = 0x3C,
    EM_TINYJ             = 0x3D,
    EM_X86_64            = 0x3E,
    EM_PDSP              = 0x3F,
    EM_PDP10             = 0x40,
    EM_PDP11             = 0x41,
    EM_FX66              = 0x42,
    EM_ST9PLUS           = 0x43,
    EM_ST7               = 0x44,
    EM_68HC16            = 0x45,
    EM_68HC11            = 0x46,
    EM_68HC08            = 0x47,
    EM_68HC05            = 0x48,
    EM_SVX               = 0x49,
    EM_ST19              = 0x4A,
    EM_VAX               = 0x4B,
    EM_CRIS              = 0x4C,
    EM_JAVELIN           = 0x4D,
    EM_FIREPATH          = 0x4E,
    EM_ZSP               = 0x4F,
    EM_MMIX              = 0x50,
    EM_HUANY             = 0x51,
    EM_PRISM             = 0x52,
    EM_AVR               = 0x53,
    EM_FR30              = 0x54,
    EM_D10V              = 0x55,
    EM_D30V              = 0x56,
    EM_V850              = 0x57,
    EM_M32R              = 0x58,
    EM_MN10300           = 0x59,
    EM_MN10200           = 0x5A,
    EM_PJ                = 0x5B,
    EM_OPENRISC          = 0x5C,
    EM_ARC_COMPACT       = 0x5D,
    EM_XTENSA            = 0x5E,
    EM_VIDEOCORE         = 0x5F,
    EM_TMM_GPP           = 0x60,
    EM_NS32K             = 0x61,
    EM_TPC               = 0x62,
    EM_SNP1K             = 0x63,
    EM_ST200             = 0x64,
    EM_IP2K              = 0x65,
    EM_MAX               = 0x66,
    EM_CR                = 0x67,
    EM_F2MC16            = 0x68,
    EM_MSP430            = 0x69,
    EM_BLACKFIN          = 0x6A,
    EM_SE_C33            = 0x6B,
    EM_SEP               = 0x6C,
    EM_ARCA              = 0x6D,
    EM_UNICORE           = 0x6E,
    EM_EXCESS            = 0x6F,
    EM_DXP               = 0x70,
    EM_ALTERA_NIOS2      = 0x71,
    EM_CRX               = 0x72,
    EM_XGATE             = 0x73,
    EM_C166              = 0x74,
    EM_M16C              = 0x75,
    EM_DSPIC30F          = 0x76,
    EM_CE                = 0x77,
    EM_M32C              = 0x78,
    EM_TSK3000           = 0x83,
    EM_RS08              = 0x84,
    EM_SHARC             = 0x85,
    EM_ECOG2             = 0x86,
    EM_SCORE7            = 0x87,
    EM_DSP24             = 0x88,
    EM_VIDEOCORE3        = 0x89,
    EM_LATTICEMICO32     = 0x8A,
    EM_SE_C17            = 0x8B,
    EM_TI_C6000          = 0x8C,
    EM_TI_C2000          = 0x8D,
    EM_TI_C5500          = 0x8E,
    EM_TI_ARP32          = 0x8F,
    EM_TI_PRU            = 0x90,
    EM_MMDSP_PLUS        = 0xA0,
    EM_CYPRESS_M8C       = 0xA1,
    EM_R32C              = 0xA2,
    EM_TRIMEDIA          = 0xA3,
    EM_QDSP6             = 0xA4,
    EM_8051              = 0xA5,
    EM_STXP7X            = 0xA6,
    EM_NDS32             = 0xA7,
    EM_ECOG1             = 0xA8,
    EM_ECOG1X            = 0xA8, /* alias */
    EM_MAXQ30            = 0xA9,
    EM_XIMO16            = 0xAA,
    EM_MANIK             = 0xAB,
    EM_CRAYNV2           = 0xAC,
    EM_RX                = 0xAD,
    EM_METAG             = 0xAE,
    EM_MCST_ELBRUS       = 0xAF,
    EM_ECOG16            = 0xB0,
    EM_CR16              = 0xB1,
    EM_ETPU              = 0xB2,
    EM_SLE9X             = 0xB3,
    EM_L10M              = 0xB4,
    EM_K10M              = 0xB5,
    EM_AARCH64           = 0xB7,
    EM_AVR32             = 0xB9,
    EM_STM8              = 0xBA,
    EM_TILE64            = 0xBB,
    EM_TILEPRO           = 0xBC,
    EM_MICROBLAZE        = 0xBD,
    EM_CUDA              = 0xBE,
    EM_TILEGX            = 0xBF,
    EM_CLOUDSHIELD       = 0xC0,
    EM_COREA_1ST         = 0xC1,
    EM_COREA_2ND         = 0xC2,
    EM_ARC_COMPACT2      = 0xC3,
    EM_OPEN8             = 0xC4,
    EM_RL78              = 0xC5,
    EM_VIDEOCORE5        = 0xC6,
    EM_78KOR             = 0xC7,
    EM_56800EX           = 0xC8,
    EM_BA1               = 0xC9,
    EM_BA2               = 0xCA,
    EM_XCORE             = 0xCB,
    EM_MCHP_PIC          = 0xCC,
    EM_INTEL205          = 0xCD,
    EM_INTEL206          = 0xCE,
    EM_INTEL207          = 0xCF,
    EM_INTEL208          = 0xD0,
    EM_INTEL209          = 0xD1,
    EM_KM32              = 0xD2,
    EM_KMX32             = 0xD3,
    EM_KMX16             = 0xD4,
    EM_KMX8              = 0xD5,
    EM_KVARC             = 0xD6,
    EM_CDP               = 0xD7,
    EM_COGE              = 0xD8,
    EM_COOL              = 0xD9,
    EM_NORC              = 0xDA,
    EM_CSR_KALIMBA       = 0xDB,
    EM_Z80               = 0xDC,
    EM_VISIUM            = 0xDD,
    EM_FT32              = 0xDE,
    EM_MOXIE             = 0xDF,
    EM_AMDGPU            = 0xE0,
    EM_RISCV             = 0xF3
} elf_machine_e;


// ABI VERSION
#define ABIVERSION_UNSPECIFIED  0x0


// ELF IDENTIFICATION

typedef struct {
  const Elf_Byte      EI_MAG0;
  const Elf_Byte      EI_MAG1;
  const Elf_Byte      EI_MAG2;
  const Elf_Byte      EI_MAG3;
  const elf_class_e   EI_CLASS;
  const elf_data_e    EI_DATA;
  const elf_version_e EI_VERSION; // must be equal to EV_CURRENT for current ELF header format, That's the ELF header version number
  const elf_osabi_e   EI_OSABI; // specifies the abi used by target os
  const Elf_Byte      EI_ABIVERSION; // version of abi, ABIVERSION_UNSPECIFIED if not specified or unspecified abi
  const Elf_Byte      EI_PAD[7]; // padding bytes for future uses
} __attribute__((packed)) e_ident_s;



// 32 BIT -------------
// header structures

// see https://www.sco.com/developers/gabi/latest/ch4.eheader.html for exact behaviour of each field
typedef struct {
        const e_ident_s           e_ident; // elf identification
        const elf_filetype_e      e_type; // object file type
        const elf_machine_e       e_machine; // required arch
        const Elf32_Word          e_version; // 0 for NONE, > 0 for the acutal version, equal to EV_CURRENT, that's the last version of a ELF header format
        const Elf32_Addr          e_entry; // entry point virutal address
        const Elf32_Off           e_phoff; // hold offset in bytes of program header table, 0 if no table 
        const Elf32_Off           e_shoff; // same for section header table
        const Elf32_Word          e_flags; // holds cpu specific flags in the format: EF_machine_name
        const Elf32_Half          e_ehsize; // ELF header size in bytes
        const Elf32_Half          e_phentsize; // size of each entry of program header table, all have the same
        const Elf32_Half          e_phnum; // number of entries in program header table, so phnum * phentsize = total table size. 0 if no table
        const Elf32_Half          e_shentsize; // same as phentsize but for section header table
        const Elf32_Half          e_shnum; // same as phnum but for section header table, if bigger than SHN_LORESERVE, 0 and actual number is located at index 0 of section header(sh_size)
        const Elf32_Half          e_shstrndx; // index, in the section header table, of section associated with section name string table
} __attribute__((packed)) Elf32_Ehdr;


// 64 BIT -------------

typedef struct {
        const e_ident_s       e_ident;
        const elf_filetype_e  e_type;
        const elf_machine_e   e_machine;
        const Elf64_Word      e_version;
        const Elf64_Addr      e_entry;
        const Elf64_Off       e_phoff;
        const Elf64_Off       e_shoff;
        const Elf64_Word      e_flags;
        const Elf64_Half      e_ehsize;
        const Elf64_Half      e_phentsize;
        const Elf64_Half      e_phnum;
        const Elf64_Half      e_shentsize;
        const Elf64_Half      e_shnum;
        const Elf64_Half      e_shstrndx;
} __attribute__((packed)) Elf64_Ehdr;

#endif
