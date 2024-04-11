#include "stack.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

void set_blocking(struct stack *s, int is_blocking)
{
    pthread_mutex_lock(&s->lock);
    s->is_blocking = is_blocking;
    pthread_mutex_unlock(&s->lock);
    pthread_cond_signal(&s->cond);
}

struct stack *stack_create(const size_t capacity)
{
    struct stack *s = mmap(
        NULL,
        sizeof(struct stack) + capacity * sizeof(struct task*),
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0);
    if (s == MAP_FAILED)
    {
        perror("mmap");
        return NULL;
    }

    s->data = (struct task *)(s + 1);
    s->capacity = capacity;
    s->top = -1;
    pthread_mutex_init(&s->lock, NULL);
    pthread_cond_init(&s->cond, NULL);

    set_blocking(s, 1);

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
    printf("dans push\n");
    while (is_full(s) && s->is_blocking)
    {
        // printf("dans while push\n");

        pthread_cond_wait(&s->cond, &s->lock);
    }
    if (is_full(s))
    {
        // printf("dans is_full push\n");

        return 0;
    }
    if (is_empty(s))
    {
        // printf("dans is_empty push\n");

        pthread_cond_signal(&s->cond);
    }
    // printf("dans push 2\n");

    pthread_mutex_lock(&s->lock);
    s->top++;
    s->data[s->top].func = func;
    s->data[s->top].closure = closure;
    pthread_mutex_unlock(&s->lock);

    return 0;
}

struct task *pop(struct stack *s)
{
    printf("debut pop\n");

    while (is_empty(s) && s->is_blocking)
    {
        printf("dans while\n");
        pthread_cond_wait(&s->cond, &s->lock);
    }
    if (is_empty(s))
    {
        printf("is_empty\n");
        return NULL;
    }
    if (is_full(s))
    {
        printf("is_full\n");
        pthread_cond_signal(&s->cond);
    }
    printf("avant mutex\n");
    pthread_mutex_lock(&s->lock);
    struct task *task = &s->data[s->top--];
    pthread_mutex_unlock(&s->lock);
    printf("fin pop\n");
    return task;
}

struct task peek(struct stack *s)
{
    if (is_empty(s))
    {
        return (struct task){NULL, NULL};
    }

    return s->data[s->top];
}

void destroy_stack(struct stack *s)
{
    munmap(s, sizeof(struct stack) + s->capacity * sizeof(void *));
    pthread_mutex_destroy(&s->lock);
}

size_t stack_size(struct stack *s)
{
    return s->top + 1;
}