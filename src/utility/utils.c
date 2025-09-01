#include <stdlib.h>
#include <string.h>
#include <utility/utils.h>

char *str_dup(const char *src, size_t len) {
  if(!len) len = strlen(src);
  char *buff = malloc(sizeof(char) * (len + 1));
  memcpy(buff, src, len);
  buff[len] = 0;
  return buff;
}

