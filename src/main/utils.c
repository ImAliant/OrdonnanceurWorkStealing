#include "utils.h"

#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

int debug = 0;
int benchmark = 0;

void *do_mmap(const size_t size, const int prot, const int flags)
{
    debugf("Mapping memory |\
    size: %zu\
    prot: %d\
    flags: %d\n", size, prot, flags);

    void *mem = mmap(NULL, size,
                     prot, flags,
                     -1, 0);
    if (mem == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    return mem;
}

void do_munmap(void *mem, const size_t size)
{
    debugf("Unmapping memory |\
    mem: %p\
    size: %zu\n", mem, size);

    if (munmap(mem, size) == -1)
    {
        perror("munmap");
        exit(EXIT_FAILURE);
    }
    mem = NULL;
}

void *do_malloc(const size_t size)
{
    debugf("Allocating memory |\
    size: %zu\n", size);

    void *mem = malloc(size);
    if (mem == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    return mem;
}

FILE *create_file(const char *filename)
{
    FILE *fp = fopen(filename, "a");
    if (fp == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        return NULL;
    }

    return fp;
}

void debugf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    if (debug)
    {
        vfprintf(stderr, format, args);
        fflush(stderr);
    }
    va_end(args);
}
