#include "sched.h"
#include "deque.h"

#include <pthread.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int sched_init_threads(int, struct scheduler *, taskfunc, void *);
int work_stealing(struct scheduler *, int);
int normal_pop(struct scheduler *, int);

struct pthread_deque
{
    pthread_t thread;
    struct deque *deque;
};

struct job_args
{
    struct scheduler *s;
    int index;
};

struct scheduler
{
    int nthreads;
    int qlen;
    struct pthread_deque *threads;
    size_t asleep_threads;
    pthread_mutex_t mutex;
};

void *job(void *arg)
{
    printf("job pthread id: %ld\n", pthread_self());
    struct job_args *args = (struct job_args *)arg;
    if (args == NULL)
    {
        return NULL;
    }
    struct scheduler *s = args->s;
    const int index = args->index;

    free(arg);

    while (1)
    {
        //printf("while job thread %ld\n", pthread_self());
        // work stealing
        if (deque_empty(s->threads[index].deque))
        {
            int need_sleep = work_stealing(s, index);
            if (need_sleep)
            {
                continue;
            }
            else
            {
                pthread_mutex_lock(&s->mutex);
                s->asleep_threads++;
                printf("threads asleep: %ld\n", s->asleep_threads);
                if (s->asleep_threads == s->nthreads) {
                    pthread_mutex_unlock(&s->mutex);
                    break;
                }
                pthread_mutex_unlock(&s->mutex);
                
                usleep(1000);

                pthread_mutex_lock(&s->mutex);
                s->asleep_threads--;
                pthread_mutex_unlock(&s->mutex);

                continue;
            }
        }
        // normal
        else
        {
            normal_pop(s, index);
        }
    }

    return NULL;
}

int increment_k(int k, int nthreads)
{
    return (k + 1) % nthreads;
}

int work_stealing(struct scheduler *s, int index)
{
    //printf("work stealing pthread id: %ld\n", pthread_self());
    srand(time(NULL));
    size_t k_initial = rand() % s->nthreads;
    if (k_initial == index)
    {
        // if k == index, we iterate to k+1 modulo nthreads
        k_initial = increment_k(k_initial, s->nthreads);
    }
    size_t k = k_initial;
    int first_iteration = 1;

    //printf("avant while\n");
    while (k != k_initial || first_iteration)
    {
        if (first_iteration)
            first_iteration = 0;
        //printf("avant pop front node\n");
        Node *node = deque_pop_front(s->threads[k].deque);
        //printf("apres pop front node\n");
        if (node == NULL)
        {
            //printf("increment k\n");
            k = increment_k(k, s->nthreads);
            continue;
        }

        //printf("avant push rear task\n");
        deque_push_rear(s->threads[index].deque, node->task);

        return 0;
    }
    //printf("apres while\n");

    return 1;
}

int normal_pop(struct scheduler *s, int index)
{
    //printf("normal pop pthread id: %ld\n", pthread_self());
    Node *node = deque_pop_rear(s->threads[index].deque);
    if (node == NULL)
    {
        return 1;
    }

    struct task *t = node->task;
    munmap(node, sizeof(Node));

    if (t == NULL)
    {
        perror("error can't be null\n");
        return 1;
    }

    t->func(t->closure, s);

    return 0;
}

int sched_init(int nthreads, int qlen, taskfunc f, void *closure)
{
    printf("sched init pthread id: %ld\n", pthread_self());
    if (nthreads == -1)
    {
        nthreads = sched_default_threads();
    }

    void *scheduler_mem = mmap(
        NULL,
        sizeof(struct scheduler) + nthreads * sizeof(struct pthread_deque),
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0);
    if (scheduler_mem == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    struct scheduler *s = (struct scheduler *)scheduler_mem;
    s->nthreads = nthreads;
    s->qlen = qlen;
    s->threads = (struct pthread_deque *)(scheduler_mem + sizeof(struct scheduler));
    s->asleep_threads = 0;
    pthread_mutex_init(&s->mutex, NULL);

    sched_init_threads(qlen, s, f, closure);

    sched_stop(s);

    return 0;
}

int sched_init_threads(int qlen, struct scheduler *s, taskfunc f, void *closure)
{
    printf("sched init threads pthread id: %ld\n", pthread_self());

    const int index_dep = 0;
    s->threads[index_dep].deque = deque_create(qlen);
    if (!s->threads[index_dep].deque)
    {
        perror("deque null");
        return 1;
    }
    struct task *task = create_task(f, closure);
    deque_push_rear(s->threads[index_dep].deque, task);

    for (int i = 0; i < s->nthreads; i++)
    {
        struct job_args *args = malloc(sizeof(struct job_args));
        if (!args)
        {
            return 1;
        }
        args->s = s;
        args->index = i;

        if (i != index_dep)
        {
            s->threads[i].deque = deque_create(qlen);
        }
        if (!s->threads[i].deque)
        {
            return 1;
        }
        pthread_create(&s->threads[i].thread, NULL, job, args);
    }

    return 0;
}

int find_thread(struct scheduler *s) {
    //printf("find thread pthread id: %ld\n", pthread_self());
    int index = -1;
    // find the thread with same identifier of the current thread
    for (int i = 0; i < s->nthreads; i++)
    {
        if (pthread_self() == s->threads[i].thread)
        {
            index = i;
            break;
        }
    }

    return index;
}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s)
{
    //printf("sched spawn pthread id: %ld\n", pthread_self());
    struct task *task = malloc(sizeof(struct task));
    if (!task)
    {
        return 1;
    }
    task->func = f;
    task->closure = closure;
    
    int index_thread = find_thread(s);
    if (index_thread == -1)
    {
        perror("error thread not found\n");
        return 1;
    }

    deque_push_rear(s->threads[index_thread].deque, task);

    return 0;
}

int sched_stop(struct scheduler *s)
{
    printf("sched stop\n");
    for (int i = 0; i < s->nthreads; i++)
    {
        pthread_join(s->threads[i].thread, NULL);
    }

    for (int i = 0; i < s->nthreads; i++)
    {
        deque_destroy(s->threads[i].deque);
    }

    munmap(s, sizeof(struct scheduler) + s->nthreads * sizeof(struct pthread_deque));

    return 0;
}
