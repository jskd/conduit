#include "../utils/bench.h"
#include <sys/socket.h>
#include <stdio.h>

void child(int fd, size_t buf_size, long long int count) {
  WRITING_LOOP(buf_size, count);
      WRITING_RET = write(fd, WRITING_BUF, WRITING_COUNT);
  END_WRITING_LOOP;
  close(fd);
  exit(0);
}

void parent(int fd, size_t buf_size) {
  READING_LOOP(buf_size) ;
    READING_RET = read(fd, READING_BUF, READING_COUNT);
  END_READING_LOOP;
  close(fd);
}

int main(int argc, char const* argv[])
{
  int fd[2];
  static const int parentsocket = 0;
  static const int childsocket = 1;
  pid_t pid;

  if ( argc < 3 ) {
    printf( "Usage: %s <buf_size> <count>\n", argv[0] );
    return 1;
  }
  ssize_t buf_size= atoi(argv[1]);
  long long int count= atoll(argv[2]);

  struct benchtime* bt= benchtime_create();
  benchtime_begin(bt);

  socketpair(PF_LOCAL, SOCK_STREAM, 0, fd);

  pid = fork();
  if (pid == 0) {
    // child
    close(fd[parentsocket]); //close parent fd
    child(fd[childsocket], buf_size, count);
  } else {
    // parent
    close(fd[childsocket]); // close child fd
    parent(fd[parentsocket], buf_size);
  }

  benchtime_end(bt);
  printf("%f\n", benchtime_result(bt));
  benchtime_destroy(bt);

  return 0;
}
