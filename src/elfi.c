#include <stdio.h>
#include <string.h>
#include <elfi.h>
#include <elf/elf.h>
#include <opt.h>
#include <utility/utils.h>
#include <error/error.h>
#include <errno.h>

elf_s  **elf_parse(char **files, elf_s **e_files) {
    // for each file
  size_t k = 0;
  for(size_t i = 0; i < FILE_MAX && files[i]; files++) {
    if(!e_files[k]) {
      e_files[k] = malloc(sizeof(elf_s));
      if(!e_files[k]) {
        // free_str_list()
        die("Memory allocation error, try again!!");
      }
    }
    e_files[k]->name = files[i];
    FILE *fd = fopen(e_files[k]->name, "r");
    if(!fd) {
      fprintf(stderr, "Cannot open file %s: %s\n", e_files[k]->name, strerror(errno));
      return NULL;
    }
    // return 0 if cannot find valid elf header(file cannot be considered as elf)
    if(!extract_header(e_files[k], fd)) {
      fclose(fd);
      continue;
    }
    // continue with section table, remember to adjust section header string table index after that
    if(!extract_sht(e_files[k], fd)) {
      fclose(fd);
      continue;
    }
    // strtb extraction
    extract_strtb(e_files[k], fd);
    k++;
    fclose(fd);
  }
  return e_files;
}



void  opt_exec(const option_s *opt, elf_s **e_files) {
  for(size_t i = 0; i < FILE_MAX && e_files[i]; i++) {
    if(opt[O_h].set) print_header_info(e_files[i]);
    if(opt[O_S].set) print_sht(e_files[i]);
  }
}

int main(int argc, char **argv) {
  char **files = malloc(sizeof(char*) * FILE_MAX);
  memset(files, 0x0, sizeof(char*) * FILE_MAX);
  if(!files) {
    die("Memory allocation error, try again!!");
  }
  __argv option_s* opt = argv_parse(PROG_OPT, argc, argv, files); // NOLINT
	if( opt[O_H].set ) argv_usage(opt, argv[0]);
  if(!*files) {
    puts("No files given!! try again");
    exit(EXIT_FAILURE);
  }
  elf_s **e_files = malloc(sizeof(elf_s*) * FILE_MAX);
  if(!e_files) {
    die("Memory allocation error, try again!!");
  }
  memset(e_files, 0x0, sizeof(elf_s*) * FILE_MAX);
  elf_parse(files, e_files);
  opt_exec(opt, e_files);
  // rember to  free single files in elf_parse and elf_s array at the end
  // free_str_list for both e_files and files
  free_str_list(files, FILE_MAX);
  free_elf_list(e_files, FILE_MAX);
  return 0;
}


// TODO
// implementare eventuali altre opts dell'elf header
// iniziare parsing section header
// ricordarsi per ogni nuova parte dell elf parsata di cambiare le free
