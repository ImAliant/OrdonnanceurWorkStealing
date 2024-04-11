#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <pthread.h>

#include "sched.h"
#include "stack.h"

struct scheduler
{
    int nthreads;
    int qlen;
    struct stack *tasks;
    pthread_t *threads;
};

int sched_init(int nthreads, int qlen, taskfunc f, void *closure)
{
    if (nthreads == -1)
    {
        nthreads = sched_default_threads();
    }

    struct scheduler *s =
        mmap(
            NULL,
            sizeof(struct scheduler),
            PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,
            -1, 0);
    if (s == MAP_FAILED)
    {
        perror("mmap");
        return -1;
    }

    s->nthreads = nthreads;
    s->qlen = qlen;
    s->tasks = stack_create(qlen);

    set_blocking(s->tasks, 0);

    push(s->tasks, f, closure);

    set_blocking(s->tasks, 1);

    printf("size of tasks: %ld\n", stack_size(s->tasks));

    start(s);

    stop(s);

    return 1;
}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s)
{
    printf("sched_spawn\n");
    if (is_full(s->tasks))
    {
        errno = EAGAIN;
        return -1;
    }

    push(s->tasks, f, closure);
    printf("pushed\n");

    return 0;
}

void *job(void *arg)
{
    printf("dans job\n");
    struct scheduler *s = (struct scheduler *)arg;
    while (1)
    {
        struct task *t = pop(s->tasks);

        if (t == NULL)
        {
            printf("t == NULL\n");
            break;
        }

        t->func(t->closure, s);
    }
    return NULL; 
}

void start(struct scheduler *s)
{
    s->threads = mmap(
        NULL,
        sizeof(pthread_t) * s->nthreads,
        PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,
        -1, 0);
    if (s->threads == MAP_FAILED)
    {
        perror("mmap start()");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < s->nthreads; i++)
    {
        pthread_create(&s->threads[i], NULL, job, s);
        printf("thread %d created\n", i);
    }
}

void stop(struct scheduler *s)
{
    for (int i = 0; i < s->nthreads; i++)
    {
        pthread_join(s->threads[i], NULL);
    }
    munmap(s->threads, sizeof(pthread_t) * s->nthreads);
    destroy_stack(s->tasks);
    munmap(s, sizeof(struct scheduler));
}
