#include "../conduct/conduct.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc, char *argv[]) {

  struct conduct* c= conduct_create(NULL, 80, 100);

  conduct_write_eof(c);

  char buffer[10];

  int count= conduct_read(c, buffer, 10);

  printf("%d", count);

  conduct_destroy(c);
  return 0;
}
