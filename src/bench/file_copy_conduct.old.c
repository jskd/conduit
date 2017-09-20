#include "../conduct/conduct.h"
#include "../utils/bench.h"

void child(struct conduct* c, const char* inputfile, size_t buf_size, long long int count) {
  READING_FILE_LOOP(inputfile, buf_size, count);
      READING_FILE_RET = conduct_write(c, READING_FILE_BUF, READING_FILE_COUNT);
  END_READING_FILE_LOOP;
  conduct_write_eof(c);
  conduct_close(c);
  exit(0);
}

void parent(struct conduct* c, const char* outputfile, size_t buf_size) {
  WRITING_FILE_LOOP(outputfile, buf_size) ;
    WRITING_FILE_RET = conduct_read(c, WRITING_FILE_BUF, WRITING_FILE_COUNT);
  END_WRITING_FILE_LOOP;
  conduct_destroy(c);
}

int main(int argc, char const* argv[])
{
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

  struct conduct* c= conduct_create(NULL, buf_size, buf_size*100);
  pid = fork();
  if (pid == 0) {
    // child
    child(c, inputfile, buf_size, count);
  } else {
    // parent
    parent(c, outputfile, buf_size);
  }

  benchtime_end(bt);
  printf("%f\n", benchtime_result(bt));
  benchtime_destroy(bt);

  return 0;
}
