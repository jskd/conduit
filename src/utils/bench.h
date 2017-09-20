#ifndef _BENCH_H_
#define _BENCH_H_

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/param.h>

struct benchtime;

struct benchtime* benchtime_create();

void benchtime_begin(struct benchtime*  b);

void benchtime_end(struct benchtime*  b);

double benchtime_result(struct benchtime* b);

void benchtime_destroy(struct benchtime* b);

#define WRITING_BUF _buf
#define WRITING_RET _nw
#define WRITING_COUNT _count

#define WRITING_LOOP(BUF_SIZE, LIMIT)  \
  char _buf[ (BUF_SIZE) ]; \
  /* write all pritable char */ \
  /* for(int _i=0; _i<(BUF_SIZE); _i++) */ \
    /*_buf[_i]= 0x20 + (_i% (0x7e - 0x20)); */ \
  const ssize_t _buf_size= (BUF_SIZE); \
  ssize_t _nw; \
  uint64_t _nw_left= (LIMIT); \
  do { \
    ssize_t _count= MIN(_buf_size, _nw_left);

//    WRITING_FILE_RET = write(socket, WRITING_FILE_BUF, WRITING_FILE_COUNT);

#define END_WRITING_LOOP  \
    if(_nw > 0) \
      _nw_left-=_nw; \
  } while(_nw_left > 0);

#define READING_BUF _buf
#define READING_RET _nr
#define READING_COUNT _buf_size

#define READING_LOOP(BUF_SIZE)  \
  char _buf[ (BUF_SIZE) ]; \
  const ssize_t _buf_size= (BUF_SIZE); \
  ssize_t _nr; \
  do {

//  READING_RET = read(fd, READING_BUF, READING_COUNT);

#define END_READING_LOOP  \
    /*write(1, _buf, _nr);*/ \
  } while(_nr > 0);

// File macro (old bench method)
// >>>

#define READING_FILE_BUF _out_buf
#define READING_FILE_RET _nw
#define READING_FILE_COUNT _out_nr

#define READING_FILE_LOOP(FILE_NAME, BUF_SIZE, LIMIT)  \
  int _fd= open (FILE_NAME, O_RDONLY); \
  char _buf[ (BUF_SIZE) ]; \
  ssize_t _nr=1; \
  ssize_t _nw; \
  uint64_t _max_r= (LIMIT); \
  while(_max_r > 0 && _nr > 0) \
  { \
    _nr= read(_fd, _buf, buf_size); \
    if(_max_r <= _nr) \
      _nr= _max_r; \
    char* _out_buf = _buf; \
    ssize_t _out_nr= _nr; \
    do {

//    READING_FILE_RET = write(socket, READING_FILE_BUF, READING_FILE_COUNT);

#define END_READING_FILE_LOOP  \
      if (_nw > 0) { \
        _out_nr -= _nw; \
        _out_buf += _nw; \
      } \
    } while (_out_nr > 0); \
    _max_r-= _nr; \
  } \
  close(_fd);

#define WRITING_FILE_BUF _buf
#define WRITING_FILE_RET _nr
#define WRITING_FILE_COUNT _buf_size

#define WRITING_FILE_LOOP(FILE_NAME, BUF_SIZE)  \
  int _fd= open( (FILE_NAME) , O_WRONLY | O_CREAT | O_TRUNC, \
    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); \
  char _buf[ (BUF_SIZE) ]; \
  const size_t _buf_size= (BUF_SIZE); \
  ssize_t _nr=1; \
  ssize_t _nw; \
  while(_nr > 0) \
  { \
//  WRITING_FILE_RET = read(fd, WRITING_FILE_BUF, WRITING_FILE_COUNT);

#define END_WRITING_FILE_LOOP  \
    char* _out_buf = _buf; \
    ssize_t _out_nr= _nr; \
    do { \
      _nw = write(_fd, _out_buf, _out_nr); \
      if (_nw > 0) { \
        _out_nr -= _nw; \
        _out_buf += _nw; \
      } \
    } while (_out_nr > 0); \
  } \
  close(_fd);

// <<<

#endif
