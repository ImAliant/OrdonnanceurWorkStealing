#include <stdio.h>
#include <stdlib.h>

#include "sched.h"
#include "deque_test.h"

void test(void *closure, struct scheduler *s)
{
    printf("test\n");
}

void deque_caracteristics(struct deque *d)
{
    printf("capacity: %ld\n", d->capacity);
    printf("size: %ld\n", d->size);
    printf("front: %p\n", d->front);
    printf("rear: %p\n", d->rear);

    deque_print(d);
}

int main(int argc, char **argv)
{
    struct deque *deque = deque_create(10);

    deque_caracteristics(deque);

    deque_push_front(deque, 22);
    deque_caracteristics(deque);

    deque_push_rear(deque, 45);
    deque_caracteristics(deque);

    deque_push_front(deque, 60);
    deque_caracteristics(deque);

    deque_pop_front(deque);
    deque_pop_rear(deque);
    deque_caracteristics(deque);

    deque_destroy(deque);
    
    return EXIT_SUCCESS;
}