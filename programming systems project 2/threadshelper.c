#include "helperhw2.h"

void* routine(void* arg)
{
    Worker* my_work = NULL;
    Thread_args* arguments = (Thread_args*)arg;
    int thread_id = arguments->thread_id;
    int log_enabled = arguments->log_enabled;
    struct timeval prog_start_time;
    prog_start_time.tv_sec = arguments->prog_start_time->tv_sec;
    prog_start_time.tv_usec = arguments->prog_start_time->tv_usec;

    free(arg);

    // Create thread log file
    if (log_enabled == 1)
        create_thread_logfile(thread_id);

    do
    {
        // lock the queue mutex to deny other threads access to the global work queue
        pthread_mutex_lock(&queue_mutex);

        while ((my_work = find_available_job()) == NULL) // wait for available job
            pthread_cond_wait(&cond, &queue_mutex);      // wait for broadcast from the dispatcher

        my_work->state = taken; // mark the found available job as 'taken'

        // unlock the queue mutex to give other threads access to the global work queue
        pthread_mutex_unlock(&queue_mutex);

        execeute_worker(my_work, thread_id, log_enabled, &prog_start_time); // execute work

        my_work->state = done; // mark finished job as 'done'

    } while (my_work->kill == FALSE); // continue looping until get 'kill' job

    return NULL;
}

Worker* find_available_job()
{
    Worker* my_work = work_queue;

    while (my_work != NULL) // search for available job
    {
        if (my_work->state == available) // found available job
            return my_work;

        my_work = my_work->next;
    }
    return NULL;
}

void execeute_worker(Worker* my_work, int thread_id, int log_enabled, struct timeval* start_time)
{
    Basic_CMD* cur_cmd = my_work->commands;
    Basic_CMD* rep_start = NULL;
    int rep_count = 0;

    // append START timing data to thread log file
    if (log_enabled == 1 && my_work->kill == FALSE)
        append_thread_log(my_work, thread_id, start_time, "START");

    while (cur_cmd != NULL)
    {
        if (!strcmp(cur_cmd->command, "msleep")) //  basic command - msleep
        {
            usleep(1000 * cur_cmd->num);
        }
        else if (!strcmp(cur_cmd->command, "increment")) //  basic command - increment
        {
            inc_dec_counter(cur_cmd->num, inc);
        }
        else if (!strcmp(cur_cmd->command, "decrement")) //  basic command - decrement
        {
            inc_dec_counter(cur_cmd->num, dec);
        }
        else if (!strcmp(cur_cmd->command, "repeat")) //  basic command - repeat
        {
            rep_start = cur_cmd->next;
            rep_count = cur_cmd->num - 1;
        }

        cur_cmd = cur_cmd->next;

        if (cur_cmd == NULL && rep_count > 0) // repeat loop
        {
            cur_cmd = rep_start;
            rep_count--;
        }
    }

    // append END timing data to thread log file
    if (log_enabled == 1 && my_work->kill == FALSE)
        append_thread_log(my_work, thread_id, start_time, "END");

    // update work's end turnaround time
    gettimeofday(&my_work->end_turnaround_time, NULL);
}

pthread_t* init_threads(int num_threads, int log_enabled, struct timeval* prog_start_time)
{
    int i, error;
    pthread_t* threads = NULL;
    Thread_args* arg;

    threads = (pthread_t*)malloc(sizeof(pthread_t) * num_threads);
    if (threads == NULL)
    {
        printf("[ERROR] malloc failed\n");
        exit(-1);
    }

    for (i = 0; i < num_threads; i++)
    {
        // allocate memory to pass the argument struct to the thread routine
        arg = (Thread_args*)malloc(sizeof(Thread_args));
        if (arg == NULL)
        {
            printf("[ERROR] malloc failed\n");
            exit(-1);
        }
        arg->thread_id = i;
        arg->log_enabled = log_enabled;
        arg->prog_start_time = prog_start_time;

        error = pthread_create(&threads[i], NULL, &routine, (void*)arg);
        if (error != 0)
        {
            printf("[ERROR] pthread_create failed: error %d - %s\n", error, strerror(error));
            exit(-1);
        }
    }

    return threads;
}

void wait_threads(pthread_t* threads, int num_threads)
{
    int i, error;
    for (i = 0; i < num_threads; i++)
    {
        error = pthread_join(threads[i], NULL);
        if (error != 0)
        {
            printf("[ERROR] pthread_join failed: error %d - %s\n", error, strerror(error));
            exit(-1);
        }
    }
    free(threads);
}

// Insert num_threads thread killers jobs to the global work queue.
void kill_threads(int num_threads)
{
    int i;
    Worker* new_worker = NULL;

    for (i = 0; i < num_threads; i++)
    {
        new_worker = create_worker_node(NULL, TRUE); // create thread killer worker
        work_queue = append_worker_to_queue(new_worker);
        pthread_cond_broadcast(&cond);
    }
}

// Wait for all pending works to get done.
void Wait_for_pending_workers()
{
    Worker* cur_work = work_queue;

    while (cur_work != NULL)
    {
        while (cur_work->state != done); // wait until work state is 'done'
        cur_work = cur_work->next;
    }
}