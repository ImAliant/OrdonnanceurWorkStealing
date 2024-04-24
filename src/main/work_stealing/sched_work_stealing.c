#include "sched_work_stealing.h"
#include "deque.h"
#include "../utils.h"
#include "../benchmark.h"

#include <pthread.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int sched_init_threads(struct scheduler *, taskfunc, void *);
int sched_init_deque(struct scheduler *, const size_t);
int sched_launch_pthread(struct scheduler *);
int sched_work_stealing(struct scheduler *, const int);
int sched_normal_pop(struct scheduler *, const int);

struct job_args
{
    struct scheduler *s;
    int index;
};

void increment_smt_task(struct scheduler *s)
{
    pthread_mutex_lock(&s->mutex);
    s->smt_task++;
    pthread_mutex_unlock(&s->mutex);
}

void decrement_smt_task(struct scheduler *s)
{
    pthread_mutex_lock(&s->mutex);
    s->smt_task--;
    pthread_mutex_unlock(&s->mutex);
}

int is_smt_task_full(struct scheduler *s)
{
    pthread_mutex_lock(&s->mutex);
    if (s->smt_task + 1 == s->qlen)
    {
        pthread_mutex_unlock(&s->mutex);
        return 1;
    }
    pthread_mutex_unlock(&s->mutex);
    return 0;
}

struct scheduler *create_scheduler(const unsigned nthreads, const unsigned qlen)
{
    debugf("Scheduler creation with %d threads\n", nthreads);
    void *scheduler_mem = do_mmap(
        sizeof(struct scheduler),
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS);

    struct scheduler *s = (struct scheduler *)scheduler_mem;
    s->nthreads = nthreads;
    s->qlen = qlen;
    s->smt_task = 0;
    s->threads = do_mmap(
        nthreads * sizeof(struct pthread_deque),
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS);

    s->asleep_threads = 0;
    pthread_mutex_init(&s->mutex, NULL);

    return s;
}

void *job(void *arg)
{
    debugf("Function job for the thread %ld\n", pthread_self());
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
        if (s->break_all)
        {
            break;
        }

        int found = sched_normal_pop(s, index);
        if (!found)
        {
            int sleep = sched_work_stealing(s, index);
            if (sleep)
            {
                pthread_mutex_lock(&s->mutex);
                s->asleep_threads++;

                if (s->asleep_threads == s->nthreads)
                {
                    s->break_all = 1;
                }
                pthread_mutex_unlock(&s->mutex);

                usleep(1000);

                pthread_mutex_lock(&s->mutex);
                s->asleep_threads--;
                pthread_mutex_unlock(&s->mutex);
            }
        }
    }
    return NULL;
}

int increment_k(int k, int nthreads)
{
    return (k + 1) % nthreads;
}

size_t generate_random_k(const int index, const unsigned nthreads)
{
    srand(time(NULL));
    size_t k = rand() % nthreads;
    if (k == index)
    {
        k = increment_k(k, nthreads);
    }
    return k;
}

int sched_work_stealing(struct scheduler *s, const int index)
{
    const size_t k_initial = generate_random_k(index, s->nthreads);
    size_t k = k_initial;
    int first_iteration = 1;

    while (k != k_initial || first_iteration)
    {
        if (first_iteration)
            first_iteration = 0;

        Node *node = deque_pop_front(s->threads[k].deque);
        if (node == NULL)
        {
            k = increment_k(k, s->nthreads);

            s->threads[index].benchmark.task_work_stealing_failed_count++;

            continue;
        }

        struct task *t = node->task;

        free(node);

        t->func(t->closure, s);

        s->threads[index].benchmark.task_completed_count++;
        s->threads[index].benchmark.task_work_stealing_completed_count++;

        free(t);

        return 0;
    }

    return 1;
}

int sched_normal_pop(struct scheduler *s, const int index)
{
    Node *node = deque_pop_rear(s->threads[index].deque);
    if (node == NULL)
    {
        return 0;
    }

    struct task *t = node->task;
    free(node);

    if (t == NULL)
    {
        perror("task can't be null");
        exit(EXIT_FAILURE);
    }

    t->func(t->closure, s);

    s->threads[index].benchmark.task_completed_count++;

    free(t);

    return 1;
}

int sched_init(int nthreads, int qlen, taskfunc f, void *closure)
{
    debugf("sched_init | \
            nthreads: %d \
            qlen: %d \
            taskfunc: %p \
            closure: %p\n", 
            nthreads, qlen, f, closure);

    if (nthreads == -1)
    {
        nthreads = sched_default_threads();
    }

    struct scheduler *s = create_scheduler(nthreads, qlen);

    sched_init_threads(s, f, closure);

    sched_stop(s);

    return 0;
}

int sched_init_threads(struct scheduler *s, taskfunc f, void *closure)
{
    const size_t index_dep = 0;
    s->threads[index_dep].deque = deque_create();
    if (!s->threads[index_dep].deque)
    {
        perror("deque null");
        exit(EXIT_FAILURE);
    }

    struct task *task = create_task(f, closure);
    deque_push_rear(s->threads[index_dep].deque, task);

    sched_init_deque(s, index_dep);

    sched_launch_pthread(s);

    return 0;
}

int sched_init_deque(struct scheduler *s, const size_t index_dep)
{
    debugf("Initializing deque\n");
    for (int i = 0; i < s->nthreads; i++)
    {
        if (i != index_dep)
        {
            s->threads[i].deque = deque_create();
        }
        if (!s->threads[i].deque)
        {
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

int sched_launch_pthread(struct scheduler *s)
{
    for (int i = 0; i < s->nthreads; i++)
    {
        struct job_args *args = malloc(sizeof(struct job_args));
        if (!args)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        args->s = s;
        args->index = i;

        pthread_create(&s->threads[i].thread, NULL, job, args);
    }
    debugf("All threads launched\n");

    return 0;
}

int sched_find_thread(struct scheduler *s)
{
    int index = -1;
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
    struct task *task = do_malloc(sizeof(struct task));

    task->func = f;
    task->closure = closure;

    int index_thread = sched_find_thread(s);
    if (index_thread == -1)
    {
        perror("error thread not found");
        exit(EXIT_FAILURE);
    }

    if (is_smt_task_full(s))
    {
        perror("too much task at the same time");
        exit(EXIT_FAILURE);
    }

    increment_smt_task(s);
    deque_push_rear(s->threads[index_thread].deque, task);
    decrement_smt_task(s);

    return 0;
}

int write_results(struct scheduler *s, int total_task_completed, 
    int total_task_work_stealing_completed, 
    int total_task_work_stealing_failed)
{
    FILE *fp = fopen("result.txt", "a");
    if (fp == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    fprintf(fp, "%s %ld %d %d %d\n", 
        "WS", s->nthreads, total_task_completed, 
        total_task_work_stealing_completed,
        total_task_work_stealing_failed);
    for (int i = 0; i < s->nthreads; i++)
    {
        fprintf(fp, "%d %d %d %d\n", 
            i, s->threads[i].benchmark.task_completed_count, 
            s->threads[i].benchmark.task_work_stealing_completed_count,
            s->threads[i].benchmark.task_work_stealing_failed_count);
    }

    fclose(fp);

    return 0;
}

int sched_stop(struct scheduler *s)
{
    debugf("Stopping scheduler\n");

    int total_task_completed = 0;
    int total_task_work_stealing_completed = 0;
    int total_task_work_stealing_failed = 0;

    for (int i = 0; i < s->nthreads; i++)
    {
        pthread_join(s->threads[i].thread, NULL);
        
        total_task_completed += s->threads[i].benchmark.task_completed_count;
        total_task_work_stealing_completed += s->threads[i].benchmark.task_work_stealing_completed_count;
        total_task_work_stealing_failed += s->threads[i].benchmark.task_work_stealing_failed_count;
    }

    for (int i = 0; i < s->nthreads; i++)
    {
        debugf("Destroying deque %d\n", i);
        deque_destroy(s->threads[i].deque);
    }

    write_results(s, total_task_completed, total_task_work_stealing_completed, total_task_work_stealing_failed);

    munmap(s, sizeof(struct scheduler) + s->nthreads * sizeof(struct pthread_deque));
    debugf("Scheduler stopped\n");

    return 0;
}
