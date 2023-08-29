#include "helperhw2.h"

// global variables
Worker* work_queue = NULL;
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int main(int argc, char* argv[])
{
    int num_threads, num_counters, log_enabled;
    pthread_t* threads = NULL;
    struct timeval prog_start_time;

    gettimeofday(&prog_start_time, NULL);

    // check command line arguments
    if (argc != 5)
    {
        printf("[ERROR] Invalid command line arguments: Incorrect number of given parameters\n");
        return -1;
    }
    num_threads = atoi(argv[2]);
    if (num_threads > MAX_THREADS || num_threads < 1)
    {
        printf("[ERROR] Invalid command line arguments: num_threads (%d) range is [1, %d]\n", num_threads, MAX_THREADS);
        return -1;
    }
    num_counters = atoi(argv[3]);
    if (num_counters > MAX_COUNTERS || num_counters < 0)
    {
        printf("[ERROR] Invalid command line arguments: num_counters (%d) range is [0, %d]\n", num_counters, MAX_COUNTERS);
        return -1;
    }
    log_enabled = atoi(argv[4]);
    if (log_enabled != 0 && log_enabled != 1)
    {
        printf("[ERROR] Invalid command line arguments: log_enabled (%d) range is [0, 1]'\n", log_enabled);
        return -1;
    }

    // Initialization
    init_counter_files(num_counters);
    threads = init_threads(num_threads, log_enabled, &prog_start_time);

    // Dispatcher
    dispatcher(argv[1], log_enabled, &prog_start_time);

    // Clean up
    kill_threads(num_threads);
    wait_threads(threads, num_threads);
    delete_queue_and_write_stats(&prog_start_time);

    return 0;
}