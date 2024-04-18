#include <pthread.h>

struct stack
{
    int *data;
    int top;
    unsigned capacity;
    int is_blocking;
    pthread_mutex_t lock;
    pthread_cond_t cond_full;
    pthread_cond_t cond_empty;
};

struct stack *stack_create(const unsigned);
void stack_destroy(struct stack *);
void stack_push(struct stack *, const int);
int stack_pop(struct stack *);
int stack_full(struct stack *);
int stack_empty(struct stack *);
void stack_print(struct stack *);
void stack_state(struct stack *);
void stack_blocking(struct stack *, const int);


