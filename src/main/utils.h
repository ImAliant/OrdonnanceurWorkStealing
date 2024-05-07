#include <stdlib.h>
#include <stdio.h>

/* debug variable */
extern int debug;
extern int benchmark;
extern int optimize_ws;

void *do_mmap(const size_t, const int, const int);
void do_munmap(void *, const size_t);
void *do_malloc(const size_t);
FILE *create_file(const char *);
void debugf(const char *, ...);