#include <stdio.h>
#include <stdlib.h>

typedef void (*taskfunc)(void*);

struct args {
    int data;
};

struct args *new_args(int data) {
    struct args *args = malloc(sizeof(struct args));
    if (args == NULL) return NULL;

    args->data = data;

    return args;
}

void test(void *closure) {
    struct args *args = (struct args *)closure;
    int data = args->data;

    free(closure);

    printf("%d\n", data);
}

int main (int argc, char **argv)
{
    taskfunc func = test;
    void *closure = new_args(10);

    func(closure);

    return 0;
}