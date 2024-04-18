#include "deque.h"
#include "mmap_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

Node *create_node(struct task *task)
{
    void *node_mem = do_mmap(
        sizeof(Node),
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS
    );

    Node *new_node = (Node *)node_mem;

    new_node->task = task;
    new_node->next = NULL;
    new_node->prev = NULL;

    return new_node;
}

struct task *create_task(taskfunc f, void *closure) {
    struct task *t = malloc(sizeof(struct task));
    if (t == NULL)
    {
        perror("malloc task\n");
        exit(EXIT_FAILURE);
    }

    t->func = f;
    t->closure = closure;
    
    return t;
}

struct deque *deque_create()
{
    void *deque_mem = do_mmap(
        sizeof(struct deque),
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS
    );

    struct deque *d = (struct deque *)deque_mem;

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
        free(temp->task);
        munmap(temp, sizeof(Node));
    }

    pthread_mutex_destroy(&d->lock);
    munmap(d, sizeof(struct deque));
}

void deque_push_front(struct deque *d, struct task *task)
{
    //printf("push front pthread id: %ld\n", pthread_self());
    pthread_mutex_lock(&d->lock);

    Node *new_node = create_node(task);
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

    /* printf("push front\n");
    deque_print_caracteristics(d); */

    pthread_mutex_unlock(&d->lock);
}

void deque_push_rear(struct deque *d, struct task *task)
{
    //printf("push rear pthread id: %ld\n", pthread_self());
    pthread_mutex_lock(&d->lock);

    Node *new_node = create_node(task);

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

    /* printf("push rear\n");
    deque_print_caracteristics(d); */

    pthread_mutex_unlock(&d->lock);
}

Node *deque_pop_front(struct deque *d)
{
    //printf("pop front pthread id: %ld\n", pthread_self());
    if (!d)
    {
        perror("error: deque is NULL");
        exit(EXIT_FAILURE);
    }

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

    /* printf("pop front\n");
    deque_print_caracteristics(d); */

    pthread_mutex_unlock(&d->lock);

    return node;
}

Node *deque_pop_rear(struct deque *d)
{
    //printf("pop rear pthread id: %ld\n", pthread_self());

    pthread_mutex_lock(&d->lock);

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

    d->size--;

    /* printf("pop rear\n");
    deque_print_caracteristics(d); */

    pthread_mutex_unlock(&d->lock);

    return node;
}

int deque_empty(struct deque *d)
{
    //printf("empty pthread id: %ld\n", pthread_self());

    if (!d)
    {
        perror("error: deque is NULL");
        exit(EXIT_FAILURE);
    }
    
    return d->size == 0;
}

void deque_print_caracteristics(struct deque *d)
{
    printf("deque %p\n", d);
    printf("size: %zu\n", d->size);
    printf("front: %p\n", d->front);
    printf("rear: %p\n", d->rear);
}
