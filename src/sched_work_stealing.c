#include "sched.h"
#include "deque.h"

#include <pthread.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>

int sched_init_threads(int, struct scheduler*, taskfunc, void *);

struct pthread_deque {
    pthread_t thread;
    struct deque *deque;
};

struct job_args {
    struct scheduler *s;
    int index;
};

struct scheduler {
    int nthreads;
    int qlen;
    struct pthread_deque *threads;
};

void *job(void *arg) {
    struct job_args *args = (struct job_args *)arg;
    free(args);

    /* while (1) {
        // Work stealing
        if (deque_empty(s->threads[index].deque)) {
            srand(time(NULL));
            int k = rand() % s->nthreads;
            if (k == index) {
                // if k == index, we iterate to k+1 modulo nthreads
                k = (k + 1) % s->nthreads;
            }

            struct task *t = deque_pop_front(s->threads[k].deque)->task;
            while (t == NULL) {
                // if the deque of the other thread is empty
                // we iterate to k+1 modulo nthreads
                k = (k + 1) % s->nthreads;

                t = deque_pop_front(s->threads[k].deque)->task;
            }

            t->func(t->closure, s);
        }
        else {
            struct task *t = deque_pop_rear(s->threads[index].deque)->task;
            if (t == NULL) {
                // peut etre erreur
                continue;
            }

            t->func(t->closure, s);
        }
    } */

    return NULL;
}

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

    sched_init_threads(qlen, s, f, closure);

    sched_stop(s);

    return 0;
}

int sched_init_threads(int qlen, struct scheduler *s, taskfunc f, void *closure) {
    const int index_dep = 0;
    s->threads[index_dep].deque = deque_create(qlen);
    if (!s->threads[index_dep].deque) {
        return 1;
    }
    struct task *task = malloc(sizeof(struct task));
    if (!task) {
        return 1;
    }
    task->func = f;
    task->closure = closure;
    deque_push_rear(s->threads[index_dep].deque, task);

    for (int i = 0; i < s->nthreads; i++) {
        struct job_args *args = malloc(sizeof(struct job_args));
        if (!args) {
            return 1;
        }
        args->s = s;
        args->index = i;

        pthread_create(&s->threads[i].thread, NULL, job, args);
        if (i != index_dep) {
            s->threads[i].deque = deque_create(qlen);
        }
        if (!s->threads[i].deque) {
            return 1;
        }
    }

    return 0;
}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s){
    return 1;
}

int sched_stop(struct scheduler *s){
    /* for (int i = 0; i < s->nthreads; i++) {
        pthread_join(s->threads[i].thread, NULL);
    } */

    for (int i = 0; i < s->nthreads; i++) {
        deque_destroy(s->threads[i].deque);
    }

    munmap(s, sizeof(struct scheduler) + s->nthreads * sizeof(struct pthread_deque));

    return 0;
}
