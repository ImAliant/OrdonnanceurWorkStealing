#include "deque_test.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

Node *create_node(const int data)
{
    void *node_mem = mmap(
        NULL,
        sizeof(Node),
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );
    if (node_mem == MAP_FAILED)
    {
        perror("mmap");
        return NULL;
    }

    Node *new_node = (Node *)node_mem;

    new_node->data = data;
    new_node->next = NULL;
    new_node->prev = NULL;

    return new_node;
}

struct deque *deque_create(const size_t capacity)
{
    void *deque_mem = mmap(
        NULL,
        sizeof(struct deque),
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

    pthread_mutex_init(&d->lock, NULL);

    return d;
}

void deque_destroy(struct deque *d)
{
    Node *current = d->front;
    Node *temp;

    while (current != NULL)
    {
        temp = current;
        current = current->next;
        munmap(temp, sizeof(Node));
    }

    pthread_mutex_destroy(&d->lock);
    munmap(d, sizeof(struct deque));
}

void deque_push_front(struct deque *d, const int data)
{
    printf("push front pthread id: %ld\n", pthread_self());
    pthread_mutex_lock(&d->lock);

    if (deque_full(d))
    {
        pthread_mutex_unlock(&d->lock);
        return;
    }

    Node *new_node = create_node(data);
    if (d->front == NULL) 
    {
        d->front = new_node;
        d->rear = new_node;
    } 
    else
    {
        new_node->next = d->front;
        d->front->prev = new_node;
        d->front = new_node;
    }

    d->size++;

    pthread_mutex_unlock(&d->lock);
}

void deque_push_rear(struct deque *d, const int data)
{
    printf("push rear pthread id: %ld\n", pthread_self());
    pthread_mutex_lock(&d->lock);

    if (deque_full(d))
    {
        pthread_mutex_unlock(&d->lock);
        return;
    }

    Node *new_node = create_node(data);

    if (d->rear == NULL)
    {
        d->front = new_node;
        d->rear = new_node;
    } 
    else
    {
        new_node->prev = d->rear;
        d->rear->next = new_node;
        d->rear = new_node;
    }

    d->size++;

    pthread_mutex_unlock(&d->lock);
}

Node *deque_pop_front(struct deque *d)
{
    printf("pop front pthread id: %ld\n", pthread_self());
    pthread_mutex_lock(&d->lock);

    if (d->front == NULL) // deque is empty
    {
        pthread_mutex_unlock(&d->lock);
        return NULL;
    }

    Node *node = d->front;

    if (d->front == d->rear)
    {
        d->front = NULL;
        d->rear = NULL;
    }
    else
    {
        d->front = d->front->next;
        d->front->prev = NULL;
    }
    d->size--;

    pthread_mutex_unlock(&d->lock);

    return node;
}

Node *deque_pop_rear(struct deque *d)
{
    printf("pop rear pthread id: %ld\n", pthread_self());

    if (d->rear == NULL) // deque is empty
    {
        return NULL;
    }

    Node *node = d->rear;

    if (d->rear == d->front)
    {
        d->rear = NULL;
        d->front = NULL;
    }
    else
    {
        d->rear = d->rear->prev;
        d->rear->next = NULL;
    }

    return node;
}

int deque_full(struct deque *d)
{
    printf("full pthread id: %ld\n", pthread_self());
    return d->size == d->capacity;
}

int deque_empty(struct deque *d)
{
    printf("empty pthread id: %ld\n", pthread_self());

    if (!d)
    {
        perror("error: deque is NULL");
        return 1;
    }

    return d->size == 0;
}

void deque_print(struct deque *d)
{
    Node *current = d->front;

    while (current != NULL)
    {
        printf("%d\n", current->data);
        current = current->next;
    }
}
