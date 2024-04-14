#ifndef STACK_H
#define STACK_H
#include <pthread.h>

#include "sched.h"

struct task {
    taskfunc func;
    void *closure;
};

struct stack {
    struct task *data;
    size_t capacity;
    size_t top;
    int is_blocking;
    pthread_mutex_t lock;
    pthread_cond_t cond_full;   
    pthread_cond_t cond_empty;
    int waiting_threads;
    int nthreads;
};

struct stack *stack_create(const size_t capacity, const int nthreads);
int is_full(struct stack *s);
int is_empty(struct stack *s);
int push(struct stack *s, taskfunc func, void *closure);
struct task* pop(struct stack *s);
struct task* peek(struct stack *s);
void destroy_stack(struct stack *s);
size_t stack_size(struct stack *s);
void set_blocking(struct stack *s, int is_blocking);

#endif