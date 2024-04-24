#include <stdlib.h>

/* debug variable */
extern int debug;
extern int benchmark;

void *do_mmap(const size_t, const int, const int);
void do_munmap(void *, const size_t);
void *do_malloc(const size_t);
void debugf(const char *, ...);