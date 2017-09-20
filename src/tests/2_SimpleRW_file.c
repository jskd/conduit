#include "../conduct/conduct.h"
#include <stdlib.h>
#include <string.h>

static void _one_read_write(struct conduct* c, const char* str) {
  size_t len;
  char* buffer= (char*) malloc(strlen(str));
  char* buf= buffer;

  len= 0;
  while(len < strlen(str)) {
    len +=conduct_write(c, str + len, strlen(str) - len);
  }

  len= 0;
  while(len < strlen(str)) {
    len+= conduct_read(c, buf+len, 15);
  }

  printf("%.*s\n", (int) len, buf);
  free(buffer);
}

int main (int argc, char *argv[]) {

  struct conduct* c= conduct_create("test2.conduct", 8, 16);
  _one_read_write(c, "---- TEST 2 ----") ;
  _one_read_write(c, "Simple W/R file") ;
  _one_read_write(c, "- Debut -") ;
  _one_read_write(c, "0123456789") ;
  _one_read_write(c, "abcdefghij") ;
  _one_read_write(c, "Salade de fruit") ;
  _one_read_write(c, "avec sauce BBQ") ;
  _one_read_write(c, "au hareng") ;
  _one_read_write(c, "- Fin -") ;
  conduct_destroy(c);
  return 0;
}
