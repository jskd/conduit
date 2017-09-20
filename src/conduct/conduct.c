#include "conduct.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <malloc.h>
#include <pthread.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/param.h>

/**
 * MMAP and file configuration
 */
/// Config annonymous conduct
static const int _mmap_anon_prot = PROT_READ | PROT_WRITE;
static const int _mmap_anon_flags = MAP_SHARED | MAP_ANONYMOUS;

/// Config named conduct
static const int _mmap_file_prot = PROT_READ | PROT_WRITE;
static const int _mmap_file_flags = MAP_SHARED;
static const int _fileflag_create = O_CREAT | O_RDWR;
static const int _fileflag_open = O_RDWR;
static const mode_t _filemode = S_IREAD | S_IWRITE | S_IRGRP | S_IROTH;

/**
 * MUTEX delay configuration
 */
static const unsigned int _wait_cond_delay_ms= 10000; // 10s pour le debug

/**
 * Condcut structure
 */
struct conduct {

  /// Name of conduct
  /// NULL annonymous conduct
  const char* name;

  /// Atomicity
  size_t a;
  /// Capacity of conduct
  size_t c;

  /// Mutex & condition write protection
  pthread_mutex_t mutex_w;
  pthread_cond_t cond_w;

  /// Mutex & condition read protection
  pthread_mutex_t mutex_r;
  pthread_cond_t cond_r;

  /// EoF flag
  char eof_flag;

  /**
   * Circular buffer
   * >>>
   */

  /// Read and write pointer
  char* w;
  char* r;

  /// End and begin pointer
  char* begin;
  char* end;

  /// Data of circular buffer
  char data[];

  /**
   * <<<
   * End circular buffer
   */
};

/**
 * Simple Hex Dump
 * @param [in] description
 * @param [in] data adress
 * @parem [in] lenght of data
 */
static void _hexDump (const char *desc, const void *addr, int len) {
  int i;
  unsigned char buff[17];
  unsigned char *pc = (unsigned char*)addr;

  // Output description if given.
  if (desc != NULL)
    printf ("%s:\n", desc);

  if (len == 0) {
    printf("  ZERO LENGTH\n");
    return;
  }

  if (len < 0) {
    printf("  NEGATIVE LENGTH: %i\n",len);
    return;
  }
  // Process every byte in the data.
  for (i = 0; i < len; i++) {
    // Multiple of 16 means new line (with line offset).

    if ((i % 16) == 0) {
    // Just don't print ASCII for the zeroth line.
      if (i != 0)
        printf ("  %s\n", buff);
      // Output the offset.
      printf ("  %04x ", i);
    }
    // Now the hex code for the specific character.
    printf (" %02x", pc[i]);
    // And store a printable ASCII character for later.
    if ((pc[i] < 0x20) || (pc[i] > 0x7e))
      buff[i % 16] = '.';
    else
      buff[i % 16] = pc[i];
    buff[(i % 16) + 1] = '\0';
  }
  // Pad out last line if not exactly 16 characters.
  while ((i % 16) != 0) {
    printf ("   ");
    i++;
  }
  // And print the final ASCII bit.
  printf ("  %s\n", buff);
}

/**
 * Debug conduct print
 * @param [in] conduct
 */
void debug_print_conduct(const struct conduct* c) {
  printf("name:%s | a:%lu | c:%lu | w:%lu | r:%lu | eof:%d \n",
    c->name, c->a, c->c,  c->w-c->begin, c->r-c->begin, c->eof_flag);
  _hexDump("data",  c->data, c->c+1);
}

/**
 * Return sizeof conduct with capacity
 * @param [in] capcity of conduct
 * @return sizeof conduct
 */
static size_t _sizeofconduct(size_t c) {
  return sizeof(struct conduct) + (c+1);
}

/**
 * Return sizeof conduct
 * @param [in] conduct
 * @return sizeof conduct
 */
static size_t _lenghtconduct(const struct conduct* c) {
  return sizeof(struct conduct) + (c->c+1);
}

/**
 * Return available writing space
 * @param [in] conduct
 * @return available space
 */
static size_t _getAvailableSpace(const struct conduct* c) {
  if(c->w >= c->r) {
    //     c->begin ====> c->r       c->w ====> c->end
    return ((c->r-1) - c->begin) + (c->end - c->w);
  } else {
    //     c->begin       c->w ====> c->r       c->end
    return (c->r-1) - c->w;
  }
}

/**
 * Return timespec for mutex wait
 * @param [in] m second delay
 * @return timepec
 */
static struct timespec _getTimeWait(unsigned int msec) {
   struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_nsec += msec * 1000000;
  return ts;
}

/**
 * Mutex lock / wait / unlock function
 */
/// >>>

/**
 *  Read lock
 *  @param [in] conduct
 */
static void _conduct_read_lock(struct conduct* c) {
  pthread_mutex_lock(&c->mutex_r);
}


/**
 *  Write lock
 *  @param [in] conduct
 */
static void _conduct_write_lock(struct conduct* c) {
  pthread_mutex_lock(&c->mutex_w);
}

/**
 * Read wait
 * @param [in] conduct
 */
static void _conduct_read_wait(struct conduct* c) {
  struct timespec ts = _getTimeWait(_wait_cond_delay_ms);
  pthread_cond_timedwait(&c->cond_r, &c->mutex_r, &ts );
}

/**
 * Write wait
 * @param [in] conduct
 */
static void _conduct_write_wait(struct conduct* c) {
  struct timespec ts = _getTimeWait(_wait_cond_delay_ms);
  pthread_cond_timedwait(&c->cond_w, &c->mutex_w, &ts );
}

/**
 * Read unlock
 * @param [in] conduct
 */
static void _conduct_read_unlock(struct conduct* c) {
  pthread_cond_signal(&c->cond_w);
  pthread_mutex_unlock(&c->mutex_r);
}

/**
 * Write unlock
 * @param [in] conduct
 */
static void _conduct_write_unlock(struct conduct* c) {
  pthread_cond_signal(&c->cond_r);
  pthread_mutex_unlock(&c->mutex_w);
}
// <<<

/**
 * Return data lenght in circular buffer
 * @param [in] conduct
 * @return data lenght
 */
static size_t _conduct_data_lenght(const struct conduct* c) {
  return c->c - _getAvailableSpace(c);
}

/**
 * Return if conduct is empty
 * @param [in] conduct
 * @return 1 if the conduct is empty, false is not
 */
static char _conduct_empty(const struct conduct* c) {
  return c->w == c->r;
}

/**
 * Ecriture dans le buffer cirulaire
 * /!\ Posibilité d'ecriture partiel si:
 * - c->w > c->r && c->end - c->w < count
 * - Espace libre < count
 * @param [in] conduct
 * @param [in] buffer
 * @param [in] count
 * @return char write
 */
static size_t _conduct_partial_write(struct conduct *c, const void* buf, size_t count) {
  size_t writable;

  if(c->w >= c->r) {
    //  c->begin       c->r       c->w ====> c->end
    writable= c->end - c->w;
  } else {
    //  c->begin       c->w ====> c->r       c->end
    writable= (c->r-1) - c->w;
  }

  if(writable == 0)
    return 0;

  writable= MIN(writable, count);
  memcpy(c->w, buf, writable);

  if(c->w+writable >= c->end)
    c->w = c->begin;
  else
    c->w += writable;

  return writable;
}

/**
 * Lecture dans le buffer cirulaire
 * /!\ Lecture partiel si:
 * - c->r > c->w && c->end - c->r < count
 * - Caractére écrit < count
 * @param [in] conduct
 * @param [out] buffer
 * @param [in] count
 * @return char read
 */
static size_t _conduct_partial_read(struct conduct *c, void* buf, size_t count) {
  size_t readable;

  if(c->w >= c->r) {
    //  c->begin       c->r ====> c->w       c->end
    readable= (c->w - c->r);
  } else {
    //  c->begin       c->w       c->r ====> c->end
    readable= (c->end - c->r);
  }

  if(readable == 0)
    return 0;

  readable= MIN(readable, count);
  memcpy(buf, c->r, readable);

  if (c->r+readable >= c->end)
    c->r = c->begin;
  else
    c->r += readable;

  return readable;
}

/**
 * Initilisation de condition partagé pour mutex
 * @param [out] condition
 */
static void _init_cond( pthread_cond_t* cond ) {
  pthread_condattr_t cond_shared;
  pthread_condattr_init(&cond_shared);
  pthread_condattr_setpshared(&cond_shared, PTHREAD_PROCESS_SHARED);
  pthread_cond_init(cond, &cond_shared);
}

/**
 * Initilisation de mutex partagé
 * @param [out] mutex
 */
static void _init_mutex( pthread_mutex_t* mutex ) {
  pthread_mutexattr_t mutex_shared;
  pthread_mutexattr_init(&mutex_shared);
  pthread_mutexattr_setpshared(&mutex_shared, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(mutex, &mutex_shared);
}

/**
 * Return total lenght of vector
 * @param [in] vector
 * @param [in] vector count
 * @return lenght of vector
 */
static size_t _iovec_total_lenght(const struct iovec *iov, int iovcnt) {
  size_t lenght= 0;

  for(int i=0; i<iovcnt; i++)
    lenght+= iov[i].iov_len;

  return lenght;
}

/**
 * Initilisation d'un conduit dans une mmap partagé dans un fichier
 * @param [in] file name
 * @param [in] atomicity of conduct
 * @param [in] capacité du conduit
 * @return conduit
 */
static struct conduct* _mmap_conduct_file(const char* name, size_t a, size_t c)
{
  if(access(name, F_OK) == 0) {
    errno= EEXIST;
    return NULL;
  }

  int fd = open(name,  _fileflag_create, _filemode);
  if(fd == -1) {
    return NULL;
  }

  if(ftruncate(fd, _sizeofconduct(c)) == -1) {
    return NULL;
  }

  struct conduct* conduct = (struct conduct*) mmap(NULL, _sizeofconduct(c), _mmap_file_prot,
    _mmap_file_flags, fd, 0);

  if (conduct == MAP_FAILED) {
    return NULL;
  }

  if(close(fd) == -1) {
    return NULL;
  }

  return conduct;
}

/**
 * Initilisation d'un conduit dans un mmap
 * @param [in] name (si null alors annonyme)
 * @param [in] atomicity of conduct
 * @param [in] capacité du conduit
 * @return conduit
 */
struct conduct* conduct_create(const char* name, size_t a, size_t c)
{
  struct conduct* conduct;

  if(name != NULL)
  {
    conduct = _mmap_conduct_file(name, a, c);
    if(conduct == NULL) {
      return NULL;
    }
  }
  else
  {
    conduct = (struct conduct*) mmap(NULL, _sizeofconduct(c), _mmap_anon_prot,
      _mmap_anon_flags, -1, 0);
    if (conduct == MAP_FAILED) {
      return NULL;
    }
  }

  conduct->c= c;
  conduct->a= a;
  conduct->name= name;
  conduct->begin= &conduct->data[0];
  conduct->end=  &conduct->data[c + 1];
  conduct->w= conduct->begin;
  conduct->r= conduct->begin;
  conduct->eof_flag= 0;
  _init_mutex(&conduct->mutex_w);
  _init_mutex(&conduct->mutex_r);
  _init_cond(&conduct->cond_w);
  _init_cond(&conduct->cond_r);

  return conduct;
}

/**
 * Ouverture d'un conduit nommé
 * @param [in] name
 * @return conduit
 */
struct conduct* conduct_open(const char* name)
{
  int fd;
  struct stat sb;

  if(name == NULL) {
    return NULL;
  }

  fd=open(name, _fileflag_open);
  if( fd == -1) {
    return NULL;
  }

  if(fstat (fd, &sb) == -1) {
    return NULL;
  }

  struct conduct* c= (struct conduct*) mmap(NULL, sb.st_size, _mmap_file_prot,
    _mmap_file_flags, -1, 0);

  if (c == MAP_FAILED) {
      return NULL;
  }

  if(close(fd) == -1) {
    return NULL;
  }

  return c;
}

/**
 * Fermeture d'un conduit
 * @param [in] conduit
 */
void conduct_close(struct conduct* c)
{
  munmap(c, _lenghtconduct(c));
}

/**
 * Destruction d'un conduit
 * @param [in] conduit
 */
void conduct_destroy(struct conduct* c)
{
  if(c->name != NULL) {
    remove(c->name);
  }

  pthread_mutex_destroy(&c->mutex_w);
  pthread_mutex_destroy(&c->mutex_r);
  pthread_cond_destroy(&c->cond_r);
  pthread_cond_destroy(&c->cond_w);

  munmap(c, _lenghtconduct(c));
}

/**
 * Read from conduct
 * @param [in] conduit
 * @param [out] buffer
 * @param [in] count read
 * @return count of reading
 */
ssize_t conduct_read(struct conduct* c, void* buf, size_t count)
{
  _conduct_read_lock(c);

  // Si le conduit est vide et sans marque de fin de fichier,
  // elle bloque jusqu’à ce qu’il ne soit plus vide ou qu’une marque de fin
  // de fichier y soit insérée.
  while( _conduct_empty( c ) &&  !c->eof_flag ) {
    _conduct_read_wait(c);
  }

  // Si le conduit est vide et contient une marque de fin de fichier, elle retourne 0.
  if(_conduct_empty(c) && c->eof_flag) {
    _conduct_read_unlock(c);
    return 0;
  }

  size_t rcount= 0;
  do {
    rcount+= _conduct_partial_read(c, buf+rcount, count-rcount);
  } while(rcount < count && _conduct_data_lenght(c) > 0);

  _conduct_read_unlock(c);

  return rcount;
}

/**
 * Write in conduct
 * @param [in] conduit
 * @param [in] buffer
 * @param [in] count write
 * @return count of writing
 */
ssize_t conduct_write(struct conduct* c, const void* buf, size_t count)
{
  _conduct_write_lock(c);

  while(
    (
      // Une écriture de n <= a octets bloque
      // jusqu’à ce qu’il reste au moins n octets libres
      (count <= c->a && _getAvailableSpace(c) < count)
      // Une écriture dans un conduit plein bloque toujours
      || _getAvailableSpace(c) == 0
    )
    // Ne bloque pas si EoF
    && !c->eof_flag
  ) {
    _conduct_write_wait(c);
  }

  // Si EoF alors la fonction conduct_write retourne -1 et errno vaut EPIPE
  if(c->eof_flag) {
    _conduct_write_unlock(c);
    errno= EPIPE;
    return -1;
  }

  size_t wcount=0;
  do {
    wcount+= _conduct_partial_write(c, buf+wcount, count-wcount);
    // On réitére si l'écriture est à la fois partiel et atomique
  } while (count <= c->a && wcount < count);

  _conduct_write_unlock(c);

  return wcount;
}

/**
 * Write in conduct
 * @return true if flag is set, false if not
 */
int conduct_write_eof(struct conduct* c)
{
  if(c->eof_flag)
    return 0;

  c->eof_flag= 1;

  pthread_cond_signal(&c->cond_w);
  pthread_cond_signal(&c->cond_r);

  return 1;
}


/**
 * Write ivector in conduct
 * @param [in] conduit
 * @param [in] vector
 * @param [in] vector count
 * @return count of writing (total char not vector)
 */
ssize_t conduct_writev(struct conduct *c, const struct iovec *iov, int iovcnt) {

  _conduct_write_lock(c);

  ssize_t count= _iovec_total_lenght(iov, iovcnt);

  while(
    (
      // Une écriture de n <= a octets bloque
      // jusqu’à ce qu’il reste au moins n octets libres
      (count <= c->a && _getAvailableSpace(c) < count)
      // Une écriture dans un conduit plein bloque toujours
      || _getAvailableSpace(c) == 0
    )
    // Ne bloque pas si EoF
    && !c->eof_flag
  ) {
    _conduct_write_wait(c);
  }

  // Si EoF alors la fonction conduct_write retourne -1 et errno vaut EPIPE
  if(c->eof_flag) {
    _conduct_write_unlock(c);
    errno= EPIPE;
    return -1;
  }

  size_t wcount=0;
  for(int i=0; i<iovcnt; i++)
  {
    // Ecriture d'un vecteur
    size_t w=0;
    do {
      w +=_conduct_partial_write(c, iov[i].iov_base+w, iov[i].iov_len-w);
      // Reiteration si atomique et ecriture partiel d'un vecteur
    } while (count <= c->a && w < iov[i].iov_len);
    wcount+= w;
    // Fin d'écriture si ecriture partiel d'un vecteur
    if(w < iov[i].iov_len)
      break;
  }

  _conduct_write_unlock(c);

  return wcount;
}

/**
 * Read ivector from conduct
 * @param [in] conduit
 * @param [out] vector
 * @param [in] vector count
 * @return count of reading (total char not vector)
 */
ssize_t conduct_readv(struct conduct *c, struct iovec *iov, int iovcnt)
{
  _conduct_read_lock(c);

  // Si le conduit est vide et sans marque de fin de fichier,
  // elle bloque jusqu’à ce qu’il ne soit plus vide ou qu’une marque de fin
  // de fichier y soit insérée.
  while( _conduct_empty( c ) &&  !c->eof_flag ) {
    _conduct_read_wait(c);
  }

  // Si le conduit est vide et contient une marque de fin de fichier, elle retourne 0.
  if(_conduct_empty(c) && c->eof_flag) {
    _conduct_read_unlock(c);
    return 0;
  }

  size_t rcount=0;
  for(int i=0; i<iovcnt; i++)
  {
    size_t r=0;
    do {
      r +=_conduct_partial_read(c, iov[i].iov_base+r, iov[i].iov_len-r);
      // Reitération si vecteur pas complet et conduit non vide
    } while (r < iov[i].iov_len && _conduct_data_lenght(c) > 0);
    rcount+= r;
    // Fin de lecture si vecteur pas complet
    if(r < iov[i].iov_len)
      break;
  }

  _conduct_read_unlock(c);

  return rcount;
}
