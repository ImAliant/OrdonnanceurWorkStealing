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

    pthread_mutex_init(&d->lock, NULL);

    return d;
}

void deque_destroy(struct deque *d)
{   
    if(deque_empty(d)) goto munmap;

    Node *tmp = d->front;

    while (tmp->next)
    {
        Node *next = tmp->next;
        free(tmp->task);
        free(tmp);
        tmp = next;
    }
    free(tmp->task);
    free(tmp);
    
    munmap:
    pthread_mutex_destroy(&d->lock);
    munmap(d, sizeof(struct deque) + d->capacity * (sizeof(Node)));
}

void deque_push_front(struct deque *d, struct task *task)
{
    printf("push front pthread id: %ld\n", pthread_self());
    pthread_mutex_lock(&d->lock);
    printf("apres lock push front pthread id: %ld\n", pthread_self());

    if (deque_full(d)) {
        printf("Can't push to full deque\n");
        pthread_mutex_unlock(&d->lock);
        return;
    }

    Node *new_node = malloc(sizeof(Node));
    if (!new_node)
    {
        perror("malloc");
        return;
    }
    new_node->task = task;
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

    pthread_mutex_unlock(&d->lock);
}

void deque_push_rear(struct deque *d, struct task *task)
{
    printf("push rear pthread id: %ld\n", pthread_self());
    pthread_mutex_lock(&d->lock);
    printf("apres lock push rear pthread id: %ld\n", pthread_self());

    if (deque_full(d)) {
        printf("Can't push to full deque\n");
        pthread_mutex_unlock(&d->lock);
        return;
    }
    
    Node *new_node = malloc(sizeof(Node));
    if (!new_node)
    {
        perror("malloc");
        return;
    }
    new_node->task = task;
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

    pthread_mutex_unlock(&d->lock);
}

Node *deque_pop_front(struct deque *d)
{
    printf("pop front pthread id: %ld\n", pthread_self());
    pthread_mutex_lock(&d->lock);
    printf("apres lock pop front pthread id: %ld\n", pthread_self());

    if (deque_empty(d)) {
        printf("Can't pop from empty deque\n");
        pthread_mutex_unlock(&d->lock);
        return NULL;
    }

    Node *node = d->front;
    Node *new_front = d->front->next;

    d->front = new_front;
    if(d->front) {
        d->front->prev = NULL;
    }
    d->size--;

    pthread_mutex_unlock(&d->lock);
    
    return node;
}

Node *deque_pop_rear(struct deque *d)
{
    printf("pop rear pthread id: %ld\n", pthread_self());

    if (deque_empty(d)) {
        printf("Can't pop from empty deque\n");
        return NULL;
    }

    Node *node = d->rear;
    Node *new_rear = d->rear->prev;
    d->rear = new_rear;
    if (d->rear){
        d->rear->next = NULL;
    }
    d->size--;
    
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
    
    if (!d){
        printf("deque is NULL\n");
        return 1;
    }
    if (d->size == 0) {
        printf("vide\n");
    }
    else printf("pas vide\n");
    return d->size == 0;
}

/* void deque_print(struct deque *d)
{
    Node *tmp = d->front;
    while (tmp->next)
    {
        printf("%d\n", tmp->data);
        tmp = tmp->next;
    }
    printf("%d\n", tmp->data);
} */
