#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "benchmark.h"
#include "utils.h"

int write_runtime_benchmark(char *filename, char *command, int nthreads, double runtime)
{
    FILE *f = create_file(filename);

    fprintf(f, "%s %d %lf\n", command, nthreads, runtime);
    fclose(f);

    return 0;
}

int write_serial_parallel_benchmark(char *filename, char *command, int n, double runtime)
{
    FILE *f = create_file(filename);

    fprintf(f, "%s %d %lf\n", command, n, runtime);
    fclose(f);

    return 0;
}