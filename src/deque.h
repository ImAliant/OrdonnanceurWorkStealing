#include "sched.h"

struct task {
    taskfunc func;
    void *closure;
};

typedef struct Node {
    int data;
    struct Node* next;
    struct Node* prev;
} Node;

struct deque {
    size_t capacity;
    size_t size;
    Node* front;
    Node* rear;
};

struct deque *deque_create(const size_t capacity);
void deque_destroy(struct deque *d);
void deque_push_front(struct deque *d, int value);
void deque_push_rear(struct deque *d, int value);
Node* deque_pop_front(struct deque *d);
Node* deque_pop_rear(struct deque *d);
int deque_full(struct deque *d);
int deque_empty(struct deque *d);
void deque_print(struct deque *d);