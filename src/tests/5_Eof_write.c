#include "../conduct/conduct.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

int main (int argc, char *argv[]) {

  struct conduct* c= conduct_create(NULL, 80, 100);

  conduct_write_eof(c);

  char buffer[10]= "salade de";

  int count= conduct_write(c, buffer, 10);

  printf("%d\n%s", count, strerror(errno));

  conduct_destroy(c);
  return 0;
}
