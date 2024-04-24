#include "../sched.h"

#include <pthread.h>

struct benchmark
{
    int task_completed_count;
    int task_work_stealing_completed_count;
    int task_work_stealing_failed_count;
};

struct pthread_deque
{
    pthread_t thread;
    struct deque *deque;

    struct benchmark benchmark;
};

struct scheduler
{
    size_t nthreads;
    size_t qlen;
    size_t smt_task;
    struct pthread_deque *threads;
    size_t asleep_threads;
    unsigned int break_all;
    pthread_mutex_t mutex;
};