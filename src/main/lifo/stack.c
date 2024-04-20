#include "stack.h"
#include "../utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

struct task *create_task(taskfunc f, void *closure)
{
    struct task *t = (struct task *)do_malloc(sizeof(struct task));
    t->func = f;
    t->closure = closure;

    return t; 
}

struct stack *stack_create(const unsigned capacity, const int nthreads)
{
    void *stack_mem = do_mmap(sizeof(struct stack),
                              PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS);
    struct stack *s = (struct stack *)stack_mem;
    s->data = (struct task **)do_mmap(capacity * sizeof(struct task),
                                      PROT_READ | PROT_WRITE,
                                      MAP_PRIVATE | MAP_ANONYMOUS);
    s->top = -1;
    s->capacity = capacity;
    s->nthreads = nthreads;
    s->waiting_threads = 0;

    pthread_mutex_init(&s->lock, NULL);
    pthread_cond_init(&s->cond_full, NULL);
    pthread_cond_init(&s->cond_empty, NULL);

    return s;
}

void stack_destroy(struct stack *s)
{
    pthread_mutex_destroy(&s->lock);
    pthread_cond_destroy(&s->cond_full);
    pthread_cond_destroy(&s->cond_empty);
    do_munmap(s->data, s->capacity * sizeof(struct task));
    do_munmap(s, sizeof(struct stack));
}

void stack_push(struct stack *s, struct task *task)
{
    pthread_mutex_lock(&s->lock);

    while (stack_full(s))
    {
        pthread_cond_wait(&s->cond_full, &s->lock);
    }
    if (stack_empty(s))
    {
        pthread_cond_broadcast(&s->cond_empty);
    }

    s->data[++s->top] = task;

    pthread_mutex_unlock(&s->lock);
}

struct task *stack_pop(struct stack *s)
{
    pthread_mutex_lock(&s->lock);

    while (stack_empty(s))
    {
        s->waiting_threads++;
        if (s->waiting_threads == s->nthreads)
        {
            pthread_cond_broadcast(&s->cond_empty);
            pthread_mutex_unlock(&s->lock);
            return NULL;
        }

        pthread_cond_wait(&s->cond_empty, &s->lock);
        s->waiting_threads--;
    }
    if (stack_full(s))
    {
        pthread_cond_broadcast(&s->cond_full);
    }

    struct task *task = s->data[s->top--];
    pthread_mutex_unlock(&s->lock);

    return task;
}

int stack_full(struct stack *s)
{
    return s->top == s->capacity - 1;
}

int stack_empty(struct stack *s)
{
    return s->top == -1;
}

void stack_state(struct stack *s)
{
    printf("stack state:\n");
    printf("top: %d\n", s->top);
    printf("capacity: %d\n", s->capacity);
    printf("waiting_threads: %d\n", s->waiting_threads);
    printf("nthreads: %d\n", s->nthreads);
}