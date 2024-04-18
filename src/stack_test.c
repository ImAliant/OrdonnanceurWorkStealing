#include "stack_test.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

struct stack *stack_create(const unsigned capacity)
{
    void *stack_mem = do_mmap(sizeof(struct stack),
                              PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS);

    struct stack *s = (struct stack *)stack_mem;
    s->data = (int *)do_mmap(capacity * sizeof(int),
                             PROT_READ | PROT_WRITE,
                             MAP_PRIVATE | MAP_ANONYMOUS);
    s->top = -1;
    s->capacity = capacity;

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
    do_munmap(s->data, s->capacity * sizeof(int));
    do_munmap(s, sizeof(struct stack));
}

void stack_push(struct stack *s, const int value)
{
    pthread_mutex_lock(&s->lock);

    while (stack_full(s) && s->is_blocking)
    {
        pthread_cond_wait(&s->cond_full, &s->lock);
    }
    if (stack_full(s))
    {
        pthread_mutex_unlock(&s->lock);
        return;
    }
    if (stack_empty(s))
    {
        pthread_cond_broadcast(&s->cond_empty);
    }

    s->data[++s->top] = value;

    pthread_mutex_unlock(&s->lock);
}

int stack_pop(struct stack *s)
{
    pthread_mutex_lock(&s->lock);

    while (stack_empty(s) && s->is_blocking) 
    {
        // TODO condition de terminaison

        pthread_cond_wait(&s->cond_empty, &s->lock);
    }
    if (stack_empty(s))
    {
        pthread_mutex_unlock(&s->lock);
        return -1;
    }
    if (stack_full(s))
    {
        pthread_cond_broadcast(&s->cond_full);
    }

    int value = s->data[s->top--];
    pthread_mutex_unlock(&s->lock);
    
    return value;
}

int stack_full(struct stack *s)
{
    return s->top == s->capacity - 1;
}

int stack_empty(struct stack *s)
{
    return s->top == -1;
}

void stack_blocking(struct stack *s, const int blocking)
{
    s->is_blocking = blocking;
}

void stack_print(struct stack *s)
{
    for (int i = 0; i <= s->top; i++)
    {
        printf("%d ", s->data[i]);
    }
    printf("\n");
}

void stack_state(struct stack *s)
{
    printf("top: %d\n", s->top);
    printf("capacity: %d\n", s->capacity);
    printf("data: %p\n", s->data);
}
