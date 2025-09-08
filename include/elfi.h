#ifndef __ELFI_H__
#define __ELFI_H__

// most number of files to parse in a single call
#define FILE_MAX  10
#include <opt.h>
#include <elf/elf.h>
// AVAILABLE OPTS
option_s PROG_OPT[] = {
	{'h', "--file-header"    , "Displays the information contained in the ELF header"               , OPT_NOARG          , 0, 0}, 
	{'H', "--help"           , "display this"                                                       , OPT_END | OPT_NOARG, 0, 0}
};


// https://www.sco.com/developers/gabi/latest/ch4.eheader.html#elfid

// opt ascii indexes in the opt array
typedef enum{
	O_h,
	O_H
}OPT_E;


// elf functions
elf_s **elf_parse(char **files, elf_s **e_files);

#endif
