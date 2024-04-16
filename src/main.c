#include <stdio.h>
#include <stdlib.h>

#include "sched.h"
#include "deque.h"

void test(void *closure, struct scheduler *s)
{
    printf("test\n");
}

int main(int argc, char **argv)
{
    /* struct deque *deque = deque_create(10);

    printf("capacity: %ld\n", deque->capacity);

    deque_push_front(deque, 22);
    printf("size: %ld\n", deque->size);
    deque_push_rear(deque, 45);
    printf("size: %ld\n", deque->size);
    deque_push_front(deque, 60);
    printf("size: %ld\n", deque->size);
    deque_pop_front(deque);
    deque_pop_rear(deque);
    printf("size: %ld\n", deque->size);

    deque_print(deque);

    deque_destroy(deque);
    printf("deque destroyed\n"); */

    sched_init(-1, 10, test, NULL);
    

    return EXIT_SUCCESS;
}