#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "benchmark.h"
#include "utils.h"

#define WS_LIFO_RUNTIME_BENCHMARK_FILE "benchmark/ws_lifo_runtime.txt"

int write_runtime_benchmark(char *command, int nthreads, double runtime)
{
    FILE *f = create_file(WS_LIFO_RUNTIME_BENCHMARK_FILE);

    fprintf(f, "%s %d %lf\n", command, nthreads, runtime);
    fclose(f);

    return 0;
}