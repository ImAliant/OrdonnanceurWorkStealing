#include "sched.h"

int write_results(struct scheduler *s, 
    int total_task_completed, 
    int total_task_work_stealing_completed, 
    int total_task_work_stealing_failed);