#include "../conduct/conduct.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc, char *argv[]) {

  struct conduct* c= conduct_create(NULL, 80, 100);

  const char *num1 = "+220130303040\n";
  const char *num2 = "+125444454555\n";
  const char *num3 = "+454457788878\n";

  size_t num_c=3;
  struct iovec num[3];

  num[0].iov_base = (char*) num1;
  num[0].iov_len = strlen(num1);

  num[1].iov_base = (char*) num2;
  num[1].iov_len = strlen(num2);

  num[2].iov_base = (char*) num3;
  num[2].iov_len = strlen(num3);

  conduct_writev(c, num, num_c);

  size_t receive_c= 3;
  struct iovec receive[ receive_c ];

  for(int i=0; i< receive_c; i++) {
     receive[i].iov_base= calloc(14, sizeof(char));
     receive[i].iov_len= 14;
  }

  conduct_readv(c, receive, receive_c);
  writev(STDOUT_FILENO, receive, receive_c);

  conduct_destroy(c);
  return 0;
}
