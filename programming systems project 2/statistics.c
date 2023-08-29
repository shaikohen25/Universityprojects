#include "helperhw2.h"

// Print worker to FILE stream (stdout for printf)
void print_worker(Worker* worker, FILE* stream)
{
    Basic_CMD* cur_cmd = worker->commands;

    if (cur_cmd != NULL)
        fprintf(stream, "worker");
    while (cur_cmd != NULL)
    {
        fprintf(stream, " %s %d;", cur_cmd->command, cur_cmd->num);
        cur_cmd = cur_cmd->next;
    }
    fprintf(stream, "\n");
}

// Create log file for thread thread_id.
void create_thread_logfile(int thread_id)
{
    FILE* fp;
    char file_name[MAX_FILENAME];

    sprintf(file_name, "thread%04d.txt", thread_id);
    fp = fopen(file_name, "w");
    if (fp == NULL)
    {
        printf("[ERROR] File \"%s\": %s\n", file_name, strerror(errno));
        exit(-1);
    }
    fclose(fp);
}

// Append timing data to thread_id log file.
void append_thread_log(Worker* my_work, int thread_id, struct timeval* start_time, char start_end[])
{
    FILE* fp;
    char file_name[MAX_FILENAME];
    long long runtime_ms;
    struct timeval end_time;

    gettimeofday(&end_time, NULL);
    runtime_ms = calc_runtime_ms(start_time, &end_time);

    sprintf(file_name, "thread%04d.txt", thread_id);
    fp = fopen(file_name, "a");
    if (fp == NULL)
    {
        printf("[ERROR] File \"%s\": %s\n", file_name, strerror(errno));
        exit(-1);
    }

    fprintf(fp, "TIME %lld: %s job ", runtime_ms, start_end);
    print_worker(my_work, fp);

    fclose(fp);
}

// Calculate runtime in ms from 2 timeval structs.
long long calc_runtime_ms(struct timeval* start_time, struct timeval* end_time)
{
    return (end_time->tv_sec - start_time->tv_sec) * 1000 + (end_time->tv_usec - start_time->tv_usec) / 1000;
}

// Create log file for the dispatcher.
void create_dispatcher_logfile()
{
    FILE* fp = fopen("dispatcher.txt", "w");
    if (fp == NULL)
    {
        printf("[ERROR] File \"%s\": %s\n", "dispatcher.txt", strerror(errno));
        exit(-1);
    }
    fclose(fp);
}

// Append timing data to the dispatcher log file.
void append_dispatcher_log(char line[], struct timeval* start_time)
{
    struct timeval end_time;
    long long runtime_ms;
    FILE* fp;

    gettimeofday(&end_time, NULL);
    runtime_ms = calc_runtime_ms(start_time, &end_time);

    fp = fopen("dispatcher.txt", "a");
    if (fp == NULL)
    {
        printf("[ERROR] File \"%s\": %s\n", "dispatcher.txt", strerror(errno));
        exit(-1);
    }

    fprintf(fp, "TIME %lld: read cmd line: %s", runtime_ms, line);

    fclose(fp);
}

// Create and write timing data to stats file, delete global work queue.
void delete_queue_and_write_stats(struct timeval* start_time)
{
    Worker* cur_work = work_queue, * next;
    long long runtime_ms;
    long long min_job = LLONG_MAX, max_job = 0, sum_jobs = 0, job_count = 0;
    float avg_job;
    struct timeval end_time;
    FILE* fp;

    while (cur_work != NULL)
    {
        // Calculate and update timing data for current node
        if (cur_work->kill == FALSE)
        {
            job_count++;
            runtime_ms = calc_runtime_ms(&cur_work->start_turnaround_time, &cur_work->end_turnaround_time);
            sum_jobs += runtime_ms;
            if (runtime_ms < min_job)
                min_job = runtime_ms;
            if (runtime_ms > max_job)
                max_job = runtime_ms;
        }

        // Delete current node
        next = cur_work->next;
        delete_commands_list(cur_work->commands);
        free(cur_work);
        cur_work = next;
    }

    avg_job = (float)sum_jobs / job_count;

    fp = fopen("stats.txt", "w");
    if (fp == NULL)
    {
        printf("[ERROR] File \"%s\": %s\n", "stats.txt", strerror(errno));
        exit(-1);
    }

    // Calcualte total runtime in ms
    gettimeofday(&end_time, NULL);
    runtime_ms = calc_runtime_ms(start_time, &end_time);

    // Print stats to file
    fprintf(fp, "total running time: %lld milliseconds\n", runtime_ms);
    fprintf(fp, "sum of jobs turnaround time: %lld milliseconds\n", sum_jobs);
    fprintf(fp, "min job turnaround time: %lld milliseconds\n", min_job);
    fprintf(fp, "average job turnaround time: %f milliseconds\n", avg_job);
    fprintf(fp, "max job turnaround time: %lld milliseconds\n", max_job);

    fclose(fp);
}