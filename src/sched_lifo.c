#include "sched.h"
#include "stack.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <pthread.h>

struct scheduler
{
    unsigned nthreads;
    unsigned qlen;
    struct stack *stack;
    pthread_t *threads;
};

int sched_launch_threads(struct scheduler *s, taskfunc f, void *closure);

struct scheduler *create_scheduler(const unsigned nthreads, const unsigned qlen)
{
    void *scheduler_mem = do_mmap(sizeof(struct scheduler),
                                  PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS);

    struct scheduler *s = (struct scheduler *)scheduler_mem;
    s->nthreads = nthreads;
    s->qlen = qlen;
    s->stack = stack_create(s->qlen, s->nthreads);
    s->threads = do_mmap(nthreads * sizeof(pthread_t),
                         PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS);

    return s;
}

int sched_init(int nthreads, int qlen, taskfunc f, void *closure)
{
    if (nthreads == -1)
    {
        nthreads = sched_default_threads();
    }

    struct scheduler *s = create_scheduler(nthreads, qlen);

    sched_launch_threads(s, f, closure);

    sched_stop(s);

    return 0;
}

void *job(void *arg)
{
    struct scheduler *s = (struct scheduler *)arg;
    if (s == NULL)
    {
        return NULL;
    }

    while (1)
    {
        struct task *t = stack_pop(s->stack);

        if (t == NULL)
        {
            break;
        }

        t->func(t->closure, s);

        free(t);
    }

    return NULL;
}

int sched_launch_threads(struct scheduler *s, taskfunc f, void *closure)
{
    struct task *t = create_task(f, closure);
    stack_push(s->stack, t);

    for (int i = 0; i < s->nthreads; i++)
    {
        pthread_create(&s->threads[i], NULL, job, s);
    }

    return 0;
}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s)
{
    struct task *t = create_task(f, closure);
    stack_push(s->stack, t);

    return 0;
}

int sched_stop(struct scheduler *s)
{
    for (int i = 0; i < s->nthreads; i++)
    {
        pthread_join(s->threads[i], NULL);
    }

    stack_destroy(s->stack);
    do_munmap(s->threads, s->nthreads * sizeof(pthread_t));
    do_munmap(s, sizeof(struct scheduler));

    return 0;
}
