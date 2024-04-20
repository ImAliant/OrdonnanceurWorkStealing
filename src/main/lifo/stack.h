#ifndef STACK_H
#define STACK_H
#include <pthread.h>

#include "../sched.h"

struct task {
    taskfunc func;
    void *closure;
};

struct stack {
    struct task **data;
    int top;
    unsigned capacity;
    pthread_mutex_t lock;
    pthread_cond_t cond_full;
    pthread_cond_t cond_empty;

    unsigned waiting_threads;
    unsigned nthreads;
};

struct task *create_task(taskfunc, void *);

struct stack *stack_create(const unsigned, const int);
void stack_destroy(struct stack *);
void stack_push(struct stack *, struct task *);
struct task *stack_pop(struct stack *);
int stack_full(struct stack *);
int stack_empty(struct stack *);
void stack_state(struct stack *);

#endif