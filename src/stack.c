#include "stack.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

void set_blocking(struct stack *s, int is_blocking)
{
    pthread_mutex_lock(&s->lock);
    s->is_blocking = is_blocking;
    pthread_mutex_unlock(&s->lock);

    pthread_cond_broadcast(&s->cond_empty);
    pthread_cond_broadcast(&s->cond_full);
}

struct stack *stack_create(const size_t capacity, const int nthreads)
{
    void *stack_mem = mmap(
        NULL,
        sizeof(struct stack) + capacity * sizeof(struct task),
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0);
    if (stack_mem == MAP_FAILED)
    {
        perror("mmap");
        return NULL;
    }

    struct stack *s = (struct stack *)stack_mem;

    s->data = (struct task *)(stack_mem + sizeof(struct stack));
    s->capacity = capacity;
    s->waiting_threads = 0;
    s->nthreads = nthreads;
    s->top = -1;
    
    pthread_mutex_init(&s->lock, NULL);
    pthread_cond_init(&s->cond_full, NULL);
    pthread_cond_init(&s->cond_empty, NULL);

    set_blocking(s, 0);     

    return s;
}

int is_full(struct stack *s)
{
    return s->top == s->capacity - 1;
}

int is_empty(struct stack *s)
{
    return s->top == -1;
}

int push(struct stack *s, taskfunc func, void *closure)
{
    pthread_mutex_lock(&s->lock);

    //printf("debut push thread %lu\n", pthread_self());
    //printf("thread %lu\n", pthread_self());
    while (is_full(s) && s->is_blocking)
    {
        //printf("thread %lu wait full\n", pthread_self());
        pthread_cond_wait(&s->cond_full, &s->lock);
    }
    if (is_full(s))
    {
        //printf("dans is_full push\n");
        pthread_mutex_unlock(&s->lock);

        return 0;
    }
    if (is_empty(s))
    {
        //printf("signal empty\n");

        pthread_cond_broadcast(&s->cond_empty);
    }

    //printf("push apres condition\n");
    //printf("push mutex\n");
    s->top++;
    s->data[s->top].func = func;
    s->data[s->top].closure = closure;
    pthread_mutex_unlock(&s->lock);
    //printf("push apres mutex\n");

    //printf("top = %ld\n", s->top);

    return 0;
}

struct task *pop(struct stack *s)
{
    //printf("debut pop\n");
    pthread_mutex_lock(&s->lock);

    while (is_empty(s) && s->is_blocking)
    {
        //printf("thread %lu wait empty\n", pthread_self());

        s->waiting_threads++;
        if (s->waiting_threads == s->nthreads) {
            // peut etre if empty
            pthread_cond_broadcast(&s->cond_empty);
            pthread_mutex_unlock(&s->lock);
            return NULL;
        }

        pthread_cond_wait(&s->cond_empty, &s->lock);
        s->waiting_threads--;
    }
    if (is_empty(s))
    {
        //printf("is_empty\n");
        pthread_mutex_unlock(&s->lock);
        return NULL;
    }
    if (is_full(s))
    {
        //printf("signal full\n");
        pthread_cond_broadcast(&s->cond_full);
    }
    //printf("pop thread %lu\n", pthread_self());

    //printf("avant mutex\n");
    struct task *task = &s->data[s->top--];
    pthread_mutex_unlock(&s->lock);
    //printf("fin pop\n");
    return task;
}

struct task *peek(struct stack *s)
{
    if (is_empty(s))
    {
        return NULL;
    }

    return &s->data[s->top];
}

void destroy_stack(struct stack *s)
{
    pthread_mutex_destroy(&s->lock);
    munmap(s, sizeof(struct stack) + s->capacity * sizeof(struct task));
}

size_t stack_size(struct stack *s)
{
    return s->top + 1;
}