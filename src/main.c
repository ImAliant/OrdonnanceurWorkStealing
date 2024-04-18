#include <stdio.h>
#include <stdlib.h>

#include "stack_test.h"

int main (int argc, char **argv)
{
    struct stack *s = stack_create(10);
    printf("s capacity: %d\n", s->capacity);

    for (int i = 0; i < 10; i++)
    {
        stack_push(s, i);
    }

    /* stack_print(s);

    stack_push(s, 25); */

    stack_print(s);
    stack_state(s);

    for (int i = 0; i < 10; i++)
    {
        printf("pop: %d\n", stack_pop(s));
        stack_print(s);
        stack_state(s);
    }

    if (stack_empty(s))
    {
        printf("stack is empty\n");
    }

    /* stack_pop(s); */

    stack_print(s);

    stack_destroy(s);

    return 0;
}