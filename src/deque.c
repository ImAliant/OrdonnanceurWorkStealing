#include "deque.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

struct deque *deque_create(const size_t capacity)
{
    void *deque_mem = mmap(
        NULL,
        sizeof(struct deque) + capacity * (sizeof(Node)),
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0);
    if (deque_mem == MAP_FAILED)
    {
        perror("mmap");
        return NULL;
    }

    struct deque *d = (struct deque *)deque_mem;

    d->capacity = capacity;
    d->size = 0;
    d->front = NULL;
    d->rear = NULL;

    return d;
}

void deque_destroy(struct deque *d)
{
    // destroy nodes
    munmap(d, sizeof(struct deque) + d->capacity * (sizeof(Node)));
}

void deque_push_front(struct deque *d, int value)
{
    if (deque_full(d))
        return;

    Node *new_node = malloc(sizeof(Node));
    if (!new_node)
    {
        perror("malloc");
        return;
    }
    new_node->data = value;
    new_node->prev = NULL;

    if (deque_empty(d))
    {
        new_node->next = NULL;
        d->rear = new_node;
    }
    else
    {
        new_node->next = d->front;
        d->front->prev = new_node;
    }

    d->front = new_node;

    d->size++;
}

void deque_push_rear(struct deque *d, int value)
{
    if (deque_full(d))
        return;
    
    Node *new_node = malloc(sizeof(Node));
    if (!new_node)
    {
        perror("malloc");
        return;
    }
    new_node->data = value;
    new_node->next = NULL;
    
    if (deque_empty(d))
    {
        new_node->prev = NULL;
        d->front = new_node;
    }
    else
    {
        new_node->prev = d->rear;
        d->rear->next = new_node;
    }

    d->rear = new_node;

    d->size++;
}

Node *deque_pop_front(struct deque *d)
{
    if (deque_empty(d)) {
        return NULL;
    }

    Node *node = d->front;
    Node *new_front = d->front->next;
    free(d->front);

    d->front = new_front;
    d->front->prev = NULL;
    d->size--;
    
    return node;
}

Node *deque_pop_rear(struct deque *d)
{
    if (deque_empty(d)) {
        return NULL;
    }

    Node *node = d->rear;
    Node *new_rear = d->rear->prev;
    free(d->rear);

    d->rear = new_rear;
    d->rear->next = NULL;
    d->size--;
    
    return node;
}

int deque_full(struct deque *d)
{
    return d->size == d->capacity;
}

int deque_empty(struct deque *d)
{
    return d->size == 0;
}

void deque_print(struct deque *d)
{
    Node *tmp = d->front;
    while (tmp->next)
    {
        printf("%d\n", tmp->data);
        tmp = tmp->next;
    }
    printf("%d\n", tmp->data);
}
