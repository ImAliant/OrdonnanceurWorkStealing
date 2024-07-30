#include "../sched.h"

#include <pthread.h>

struct task {
    taskfunc func;
    void *closure;
};

typedef struct Node {
    struct task *task;
    struct Node* next;
    struct Node* prev;
} Node;

struct deque {
    size_t size;
    Node* front;
    Node* rear;
    pthread_mutex_t lock;
};

struct task *create_task(taskfunc f, void *closure);

struct deque *deque_create();
void deque_destroy(struct deque *d);
void deque_push_front(struct deque *d, struct task *task);
void deque_push_rear(struct deque *d, struct task *task);
Node* deque_pop_front(struct deque *d);
Node* deque_pop_rear(struct deque *d);
int deque_empty(struct deque *d);
void deque_print_caracteristics(struct deque *d);