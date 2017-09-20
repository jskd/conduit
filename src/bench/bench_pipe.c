#include "../conduct/conduct.h"
#include "../utils/bench.h"

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
  pid_t pid;

  if ( argc < 3 ) {
    printf( "Usage: %s <buf_size> <count>\n", argv[0] );
    return 1;
  }
  ssize_t buf_size= atoi(argv[1]);
  long long int count= atoll(argv[2]);

  struct benchtime* bt= benchtime_create();
  benchtime_begin(bt);

  pipe(fd);

  pid = fork();
  if (pid == 0) {
    // child
    close(fd[0]);
    child(fd[1], buf_size, count);
  } else {
    // parent
    close(fd[1]);
    parent(fd[0], buf_size);
  }

  benchtime_end(bt);
  printf("%f\n", benchtime_result(bt));
  benchtime_destroy(bt);

  return 0;
}
