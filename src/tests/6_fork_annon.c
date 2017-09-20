#include "../conduct/conduct.h"
#include <stdlib.h>
#include <unistd.h>

void child(struct conduct* c) {
  conduct_write(c, "hello world!", 13);
  conduct_close(c);
  exit(0);
}

void parent(struct conduct* c) {
  char buffer[250];
  conduct_read(c, buffer, 255);
  printf("%s", buffer);
  conduct_destroy(c);
}

int main(int argc, char const* argv[])
{
  pid_t pid;

  struct conduct* c= conduct_create(NULL, 20, 100);
  pid = fork();
  if (pid == 0) {
    // child
    child(c);
  } else {
    // parent
    parent(c);
  }

  return 0;
}
