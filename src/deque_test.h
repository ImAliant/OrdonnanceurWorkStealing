#include "sched.h"

#include <pthread.h>

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
    pthread_mutex_t lock;
};

struct deque *deque_create(const size_t capacity);
void deque_destroy(struct deque *d);
void deque_push_front(struct deque *d, const int data);
void deque_push_rear(struct deque *d, const int data);
Node* deque_pop_front(struct deque *d);
Node* deque_pop_rear(struct deque *d);
int deque_full(struct deque *d);
int deque_empty(struct deque *d);
void deque_print(struct deque *d);