#include "../conduct/conduct.h"
#include "../utils/bench.h"

void child(struct conduct* c, size_t buf_size, long long int count) {
  WRITING_LOOP(buf_size, count);
      WRITING_RET = conduct_write(c, WRITING_BUF, WRITING_COUNT);
  END_WRITING_LOOP;
  conduct_write_eof(c);
  conduct_close(c);
  exit(0);
}

void parent(struct conduct* c, size_t buf_size) {
  int i=0;
  READING_LOOP(buf_size) ;
    i++;
    READING_RET = conduct_read(c, READING_BUF, READING_COUNT);
  END_READING_LOOP;
  conduct_destroy(c);
}

int main(int argc, char const* argv[])
{
  pid_t pid;

  if ( argc < 5 ) {
    printf( "Usage: %s <buf_size> <count> <conduct_a> <conduct_c>\n", argv[0] );
    return 1;
  }
  size_t buf_size= atoi(argv[1]);
  long long int count= atoll(argv[2]);
  size_t conduct_a= atoi(argv[3]);
  size_t conduct_c= atoi(argv[4]);

  struct benchtime* bt= benchtime_create();
  benchtime_begin(bt);

  struct conduct* c= conduct_create(NULL, conduct_a, conduct_c);
  pid = fork();
  if (pid == 0) {
    // child
    child(c, buf_size, count);
  } else {
    // parent
    parent(c, buf_size);
  }

  benchtime_end(bt);
  printf("%f\n", benchtime_result(bt));
  benchtime_destroy(bt);

  return 0;
}
