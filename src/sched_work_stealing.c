#include "sched.h"
#include "deque.h"

#include <pthread.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>

struct pthread_deque {
    pthread_t thread;
    struct deque *deque;
};

struct scheduler {
    int nthreads;
    int qlen;
    struct pthread_deque *threads;
};

int sched_init(int nthreads, int qlen, taskfunc f, void *closure){
    if (nthreads == -1) {
        nthreads = sched_default_threads();
    }

    void *scheduler_mem = mmap(
        NULL,
        sizeof(struct scheduler) + nthreads * sizeof(struct pthread_deque),
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );
    if (scheduler_mem == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    struct scheduler *s = (struct scheduler *)scheduler_mem;
    s->nthreads = nthreads;
    s->qlen = qlen;
    s->threads = (struct pthread_deque *)(scheduler_mem + sizeof(struct scheduler));
    if (!s->threads) {
        perror("malloc");
        return 1;
    }
    for (int i = 0; i < nthreads; i++) {
        s->threads[i].deque = deque_create(qlen);
        if (!s->threads[i].deque) {
            return 1;
        }
    }

    return 0;
}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s){
    return 1;
}

int stop(struct scheduler *s){
    for (int i = 0; i < s->nthreads; i++) {
        deque_destroy(s->threads[i].deque);
    }

    munmap(s, sizeof(struct scheduler) + s->nthreads * sizeof(struct pthread_deque));

    return 0;
}
