#ifndef HW2_H
#define HW2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
#include <limits.h>

#define MAX_THREADS 4096
#define MAX_COUNTERS 100
#define MAX_FILENAME 200
#define MAX_BASIC_CMD 20
#define MAX_BASIC_ARGS 400
#define MAX_CMD_LINE 1024
#define MAX_COUNTER_CHARS 100
#define TRUE 1
#define FALSE 0

// enums
enum worker_status { available, taken, done };
enum counter_action { inc, dec };

// structs
typedef struct Thread_args
{
    int thread_id, log_enabled;
    struct timeval* prog_start_time;
} Thread_args;

typedef struct Basic_CMD
{
    char command[MAX_BASIC_CMD];
    int num;
    struct Basic_CMD* next;
} Basic_CMD;

typedef struct Worker
{
    struct Basic_CMD* commands;
    enum worker_status state;
    int kill;
    struct timeval start_turnaround_time, end_turnaround_time;
    struct Worker* next;
} Worker;

// files
void init_counter_files(int num_counters);
void inc_dec_counter(int counter_n, enum counter_action inc_dec);

// strings
void split_line(char line[], char* args[]);

// dispatcher
void dispatcher(char cmdfilename[], int log_enabled, struct timeval* start_time);

// threads
pthread_t* init_threads(int num_threads, int log_enabled, struct timeval* prog_start_time);
void wait_threads(pthread_t* threads, int num_threads);
void* routine(void* arg);
void execeute_worker(Worker* my_work, int thread_id, int log_enabled, struct timeval* start_time);
Worker* find_available_job();
void Wait_for_pending_workers();
void insert_thread_killers(int num_threads);

// worker - basic commands linked list
Basic_CMD* create_command_node(char* command, int num);
Basic_CMD* append_cmd_to_worker(Basic_CMD* worker, Basic_CMD* new_cmd);
void delete_commands_list(Basic_CMD* commands);
Basic_CMD* array_to_cmd_list(char* args[]);

// work queue - workers linked list
Worker* create_worker_node(Basic_CMD* commands, int kill);
Worker* append_worker_to_queue(Worker* new_worker);
void delete_workers_list();

// stats
void create_thread_logfile(int thread_id);
void print_worker(Worker* worker, FILE* stream);
void append_thread_log(Worker* my_work, int thread_id, struct timeval* start_time, char start_end[]);
long long calc_runtime_ms(struct timeval* start_time, struct timeval* end_time);
void create_dispatcher_logfile();
void append_dispatcher_log(char line[], struct timeval* start_time);
void delete_queue_and_write_stats(struct timeval* start_time);


// gloabal variables
extern Worker* work_queue;
extern pthread_mutex_t file_mutex;
extern pthread_mutex_t queue_mutex;
extern pthread_cond_t cond;

#endif