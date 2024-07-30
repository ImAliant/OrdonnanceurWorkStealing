#include "../sched.h"

#include <pthread.h>

struct scheduler
{
    unsigned nthreads;
    unsigned qlen;
    struct stack *stack;
    pthread_t *threads;
};