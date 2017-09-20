#include "../conduct/conduct.h"
#include "../utils/bench.h"

void child(int fd, const char* inputfile, size_t buf_size, long long int count) {
  READING_FILE_LOOP(inputfile, buf_size, count);
      READING_FILE_RET = write(fd, READING_FILE_BUF, READING_FILE_COUNT);
  END_READING_FILE_LOOP;
  close(fd);
  exit(0);
}

void parent(int fd, const char* outputfile, size_t buf_size) {
  WRITING_FILE_LOOP(outputfile, buf_size) ;
    WRITING_FILE_RET = read(fd, WRITING_FILE_BUF, WRITING_FILE_COUNT);
  END_WRITING_FILE_LOOP;
  close(fd);
}

int main(int argc, char const* argv[])
{
   int fd[2];
  pid_t pid;

  if ( argc != 5 ) {
    printf( "Usage: %s <inputfile> <outputfile> <buf_size> <count>", argv[0] );
    return 1;
  }
  const char* inputfile= argv[1];
  const char* outputfile= argv[2];
  ssize_t buf_size= atoi(argv[3]);
  long long int count= atoll(argv[4]);

  struct benchtime* bt= benchtime_create();
  benchtime_begin(bt);

  pipe(fd);

  pid = fork();
  if (pid == 0) {
    // child
    close(fd[0]);
    child(fd[1], inputfile, buf_size, count);
  } else {
    // parent
    close(fd[1]);
    parent(fd[0], outputfile, buf_size);
  }

  benchtime_end(bt);
  printf("%f\n", benchtime_result(bt));
  benchtime_destroy(bt);

  return 0;
}
