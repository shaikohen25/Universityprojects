#include "helperhw2.h"

void dispatcher(char cmdfilename[], int log_enabled, struct timeval* start_time)
{
    char line[MAX_CMD_LINE];
    char* args[MAX_BASIC_ARGS];
    Basic_CMD* cmd_lst = NULL;
    Worker* new_worker = NULL;
    FILE* fp;

    fp = fopen(cmdfilename, "r");
    if (fp == NULL)
    {
        printf("[ERROR] File \"%s\": %s\n", cmdfilename, strerror(errno));
        exit(-1);
    }

    if (log_enabled == 1)
        create_dispatcher_logfile();

    while (fgets(line, MAX_CMD_LINE, fp) != NULL)
    {

        if (log_enabled == 1)
            append_dispatcher_log(line, start_time);

        split_line(line, args);

        if (args[0] == NULL) // skip empty line
            continue;

        if (!strcmp(args[0], "worker"))
        {
            cmd_lst = array_to_cmd_list(args);
            new_worker = create_worker_node(cmd_lst, FALSE);
            work_queue = append_worker_to_queue(new_worker);
            gettimeofday(&new_worker->start_turnaround_time, NULL);
            pthread_cond_broadcast(&cond); // wake up sleeping threads
        }
        else if (!strcmp(args[0], "dispatcher_msleep"))
        {
            usleep(1000 * atoi(args[1]));
        }
        else if (!strcmp(args[0], "dispatcher_wait"))
        {
            Wait_for_pending_workers();
        }
    }

    fclose(fp);
}

void init_counter_files(int num_counters)
{
    FILE* fp;
    int i;
    char file_name[MAX_FILENAME];

    for (i = 0; i < num_counters; i++)
    {
        sprintf(file_name, "count%02d.txt", i);
        fp = fopen(file_name, "w");
        if (fp == NULL)
        {
            printf("[ERROR] File \"%s\": %s\n", file_name, strerror(errno));
            exit(-1);
        }
        fprintf(fp, "0");
        fclose(fp);
    }
}

void inc_dec_counter(int counter_n, enum counter_action inc_dec)
{
    FILE* fp;
    char file_name[MAX_FILENAME];
    long long num;
    char num_str[MAX_COUNTER_CHARS];

    sprintf(file_name, "count%02d.txt", counter_n);

    pthread_mutex_lock(&file_mutex);

    // read from counter n
    fp = fopen(file_name, "r");
    if (fp == NULL)
    {
        printf("[ERROR] File \"%s\": %s\n", file_name, strerror(errno));
        exit(-1);
    }
    fgets(num_str, MAX_COUNTER_CHARS, fp);
    fclose(fp);

    // increase or decrease counter n
    num = atoi(num_str);
    switch (inc_dec)
    {
    case inc: num++; break;
    case dec: num--; break;
    }

    // write to counter n
    fp = fopen(file_name, "w");
    if (fp == NULL)
    {
        printf("[ERROR] File \"%s\": %s\n", file_name, strerror(errno));
        exit(-1);
    }
    fprintf(fp, "%lld", num);
    fclose(fp);

    pthread_mutex_unlock(&file_mutex);
}