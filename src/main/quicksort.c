#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#include "sched.h"
#include "utils.h"
#include "benchmark.h"

#define RUNTIME_BENCHMARK_FILE "benchmark/runtime.txt"
#define RUNTIME_OPTIMIZED_BENCHMARK_FILE "benchmark/runtime_optimized.txt"
#define SERIAL_PARALLEL_BENCHMARK_FILE "benchmark/serial_parallel.txt"

#define NORMAL_QUICKSORT_THRESHOLD 128
#define OPTIMIZED_QUICKSORT_THRESHOLD 1024
#define DEFAULT_SIZE_ARRAY 10 * 1024 * 1024

int partition(int *a, int lo, int hi)
{
    int pivot = a[lo];
    int i = lo - 1;
    int j = hi + 1;
    int t;
    while (1)
    {
        do
        {
            i++;
        } while (a[i] < pivot);

        do
        {
            j--;
        } while (a[j] > pivot);

        if (i >= j)
        {
            return j;
        }

        t = a[i];
        a[i] = a[j];
        a[j] = t;
    }
}

struct quicksort_args
{
    int *a;
    int lo, hi;
};

struct quicksort_args *
new_args(int *a, int lo, int hi)
{
    struct quicksort_args *args = malloc(sizeof(struct quicksort_args));
    if (args == NULL)
        return NULL;

    args->a = a;
    args->lo = lo;
    args->hi = hi;
    return args;
}

void quicksort_serial(int *a, int lo, int hi)
{
    int p;

    if (lo >= hi)
        return;

    p = partition(a, lo, hi);
    quicksort_serial(a, lo, p);
    quicksort_serial(a, p + 1, hi);
}

void quicksort(void *closure, struct scheduler *s)
{
    struct quicksort_args *args = (struct quicksort_args *)closure;
    int *a = args->a;
    int lo = args->lo;
    int hi = args->hi;
    int p;
    int rc;

    free(closure);

    if (lo >= hi)
    {
        return;
    }

    int threshold = optimize_ws ? OPTIMIZED_QUICKSORT_THRESHOLD : NORMAL_QUICKSORT_THRESHOLD;
    if (hi - lo <= threshold)
    {
        quicksort_serial(a, lo, hi);
        return;
    }

    p = partition(a, lo, hi);
    rc = sched_spawn(quicksort, new_args(a, lo, p), s);
    assert(rc >= 0);
    rc = sched_spawn(quicksort, new_args(a, p + 1, hi), s);
    assert(rc >= 0);
}

int main(int argc, char **argv)
{
    int *a;
    struct timespec begin, end;
    double delay;
    int rc;
    int n = DEFAULT_SIZE_ARRAY;
    int nthreads = -1;
    int serial = 0;

    while (1)
    {
        int opt = getopt(argc, argv, "ogdsn:t:");
        if (opt < 0)
            break;
        switch (opt)
        {
        case 'd':
            debug = 1;
            debugf("debugging enabled\n");
            break;
        case 'g':
            benchmark = 1;
            debugf("benchmarking enabled\n");
            break;
        case 'o':
            optimize_ws = 1;
            debugf("optimization enabled\n");
            break;
        case 's':
            serial = 1;
            break;
        case 'n':
            n = atoi(optarg);
            if (n <= 0)
                goto usage;
            break;
        case 't':
            nthreads = atoi(optarg);
            if (nthreads <= 0)
                goto usage;
            break;
        default:
            goto usage;
        }
    }

    a = malloc(n * sizeof(int));

    unsigned long long s = 0;
    for (int i = 0; i < n; i++)
    {
        s = s * 6364136223846793005ULL + 1442695040888963407;
        a[i] = (int)((s >> 33) & 0x7FFFFFFF);
    }

    clock_gettime(CLOCK_MONOTONIC, &begin);

    if (serial)
    {
        quicksort_serial(a, 0, n - 1);
    }
    else
    {
        rc = sched_init(nthreads, (n + 127) / 128,
                        quicksort, new_args(a, 0, n - 1));
        assert(rc >= 0);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    delay = end.tv_sec + end.tv_nsec / 1000000000.0 -
            (begin.tv_sec + begin.tv_nsec / 1000000000.0);
    printf("Done in %lf seconds.\n", delay);

    if (benchmark)
    {
        char *command = argv[0];
        char *filename = optimize_ws ? RUNTIME_OPTIMIZED_BENCHMARK_FILE : RUNTIME_BENCHMARK_FILE;
        write_runtime_benchmark(filename, command, nthreads, delay);

        filename = SERIAL_PARALLEL_BENCHMARK_FILE;
        write_serial_parallel_benchmark(filename, command, n, delay);
    }

    for (int i = 0; i < n - 1; i++)
    {
        assert(a[i] <= a[i + 1]);
    }

    free(a);
    return 0;

usage:
    printf("quicksort [-n size] [-t threads] [-s]\n");
    return 1;
}
