#ifndef _CONDUCT_H_
#define _CONDUCT_H_

#include <stdio.h>
#include <sys/uio.h>
#include <sys/types.h>

struct conduct;

/**
 * Initilisation d'un conduit dans un mmap
 * @param [in] name (si null alors annonyme)
 * @param [in] atomicity of conduct
 * @param [in] capacité du conduit
 * @return conduit
 */
struct conduct* conduct_create(const char* name, size_t a, size_t c);

/**
 * Ouverture d'un conduit nommé
 * @param [in] name
 * @return conduit
 */
struct conduct* conduct_open(const char* name);

/**
 * Fermeture d'un conduit
 * @param [in] conduit
 */
void conduct_close(struct conduct* c);

/**
 * Destruction d'un conduit
 * @param [in] conduit
 */
void conduct_destroy(struct conduct* c);

/**
 * Read from conduct
 * @param [in] conduit
 * @param [out] buffer
 * @param [in] count read
 * @return count of reading
 */
ssize_t conduct_read(struct conduct* c, void* buf, size_t count);

/**
 * Write in conduct
 * @param [in] conduit
 * @param [in] buffer
 * @param [in] count write
 * @return count of writing
 */
ssize_t conduct_write(struct conduct* c, const void* buf, size_t count);

/**
 * Write ivector in conduct
 * @param [in] conduit
 * @param [in] vector
 * @param [in] vector count
 * @return count of writing (total char not vector)
 */
ssize_t conduct_writev(struct conduct *c, const struct iovec *iov, int iovcnt);

/**
 * Read ivector from conduct
 * @param [in] conduit
 * @param [out] vector
 * @param [in] vector count
 * @return count of reading (total char not vector)
 */
ssize_t conduct_readv(struct conduct *c, struct iovec *iov, int iovcnt);

/**
 * Write in conduct
 * @return true if flag is set, false if not
 */
int conduct_write_eof(struct conduct* c);

/**
 * Debug conduct print
 * @param [in] conduct
 */
void debug_print_conduct(const struct conduct* c);

#endif // _CONDUCT_H_

/* Eof */
