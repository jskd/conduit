#include <sys/time.h>
#include <stdlib.h>
#include "bench.h"

typedef unsigned long long timestamp_t;

struct benchtime {
  timestamp_t begin;
  timestamp_t end;
};

static timestamp_t _get_timestamp()
{
  struct timeval now;
  gettimeofday (&now, 0);
  return  now.tv_usec + (timestamp_t) now.tv_sec * 1000000;
}

struct benchtime* benchtime_create() {
  return malloc (sizeof (struct benchtime));
}

void benchtime_begin(struct benchtime* b)
{
  b->begin= _get_timestamp();
}

void benchtime_end(struct benchtime* b)
{
  b->end= _get_timestamp();
}

double benchtime_result(struct benchtime* b)
{
  return (b->end - b->begin) / 1000000.0L;
}

void benchtime_destroy(struct benchtime* b)
{
  free(b);
}
