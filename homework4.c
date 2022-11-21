#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>

#include "queue.h"

#define PATHMAX 4096

int P, counter, job_counter = 0;
queue *queue1;
job jobs[50];
size_t MAXLENGTH = 1000;

// To take the commands entered by the user and storing them in the list of job structure.
void input_commands();
// To run the main thread that is used to execute the child thread that is responsible for running jobs.
void *run_thread_1();
// To run the child thread that is responsible for running jobs submitted by user.
void *run_thread_2(void *arg);
// To display the history of jobs that are executed or jobs that are currently running or in the waiting state.
void display_jobs(int display_type); // display_type value 0 for "showjobs" command and 1 for "submithistory" command.

int main(int argc, char **argv)
{
    pthread_t tid;

    if (argc != 2)
    {
        printf("Usage: %s <maximum_number_of_jobs_to_run>", argv[0]);
    }

    queue1 = queue_init(50);

    P = atoi(argv[1]);

    pthread_create(&tid, NULL, run_thread_1, NULL);

    input_commands();

    return 0;
}

void input_commands()
{
    counter = 0;

    printf("Welcome to the shell. Here you can run jobs in the background and display the jobs that are running and waiting. You can also see the history of jobs that are already executed.\n\nFollowing are the commands that works in this shell:\n\n1. submit <job_command> - To submit a job\n2. showjobs - To show the jobs that are running in the background or waiting in the background to run\n3. submithistory - To show the history of jobs that are already done either in 'Successfully Completed' State or 'Completed with Failure' state\n\n");

    while (1)
    {
        printf("> ");

        char *line;

        if (getline(&line, &MAXLENGTH, stdin) > 0)
        {
            char *linecopy = malloc(1000 * sizeof(char *));
            strcpy(linecopy, line);
            char *prefix = strtok(linecopy, " \n");
            if (strcmp("submit", prefix) == 0)
            {
                char *command_str = strchr(line, ' ') + 1;
                char *command = malloc(1000 * sizeof(char *));
                strcpy(command, command_str);

                strcpy(jobs[counter].input_command, command_str);
                jobs[counter].input_command[strlen(jobs[counter].input_command) - 1] = '\0';

                char *token = strtok(command, " \n");
                int i = 0;
                while (token != NULL)
                {
                    jobs[counter].args[i] = token;
                    i++;
                    token = strtok(NULL, " \n");
                }
                jobs[counter].args[i] = NULL;

                jobs[counter].job_id = counter;

                jobs[counter].status = "WAITING";

                queue_insert(queue1, jobs + counter);

                counter++;
            }
            else if (strcmp("submithistory", prefix) == 0)
            {
                display_jobs(1);
            }
            else if (strcmp("showjobs", prefix) == 0)
            {
                display_jobs(0);
            }
        }
    }
}

void *run_thread_2(void *arg)
{
    job_counter++;
    int status;
    int fd1, fd2, job_id;
    time_t time1, time2;

    job *current_job = (job *)arg;

    job_id = current_job->job_id;

    char output_file[PATHMAX], error_file[PATHMAX];

    sprintf(output_file, "%d.out", job_id);
    sprintf(error_file, "%d.err", job_id);

    fd1 = open(output_file, O_CREAT | O_WRONLY, 0755);

    if (fd1 == -1)
    {
        printf("Error opening file %s\n", output_file);
        exit(-1);
    }

    fd2 = open(error_file, O_CREAT | O_WRONLY, 0755);

    if (fd2 == -1)
    {
        printf("Error opening file %s\n", error_file);
    }

    pid_t pid = fork();

    time(&time1);
    char *time1_str = ctime(&time1);
    strtok(time1_str, "\n");
    strcpy(current_job->start_time, time1_str);
    current_job->status = "RUNNING";

    if (pid == 0)
    {

        dup2(fd1, 1);
        dup2(fd2, 2);

        execvp(current_job->args[0], current_job->args);
        perror("exec");
        exit(-1);
    }
    else if (pid > 0)
    {
        wait(&status);

        time(&time1);
        char *time2_str = ctime(&time1);
        strtok(time2_str, "\n");
        strcpy(current_job->end_time, time2_str);

        current_job->status = "SUCCESFULLY COMLPETED";

        if (WIFEXITED(status))
        {
            current_job->status = WEXITSTATUS(status) == 0 ? "SUCCESFULLY COMLPETED" : "COMPLETED WITH FAILURE";
        }
        else
            current_job->status = "COMPLETED WITH FAILURE";
    }
    else
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    job_counter--;

    return NULL;
}

void *run_thread_1()
{
    while (1)
    {
        if (queue1->count > 0 && job_counter < P)
        {
            pthread_t tid;
            job *job1;

            job1 = queue_delete(queue1);
            pthread_create(&tid, NULL, run_thread_2, job1);
        }
    }
}

void display_jobs(int display_type)
{
    int i;

    if (display_type == 0)
    {
        printf("Job Id\tCommand\tStatus\n");
        for (i = 0; i < counter; i++)
        {
            if (strcmp(jobs[i].status, "RUNNING") == 0 || strcmp(jobs[i].status, "WAITING") == 0)
                printf("%d\t%s\t%s\n", jobs[i].job_id, jobs[i].input_command, jobs[i].status);
        }
    }
    else if (display_type == 1)
    {
        printf("Job Id\tCommand\tStart Time\tEnd Time\tStatus\n");
        for (i = 0; i < counter; i++)
        {
            if (strcmp(jobs[i].status, "SUCCESFULLY COMLPETED") == 0 || strcmp(jobs[i].status, "COMPLETED WITH FAILURE") == 0)
                printf("%d\t%s\t%s\t%s\t%s\n", jobs[i].job_id, jobs[i].input_command, jobs[i].start_time, jobs[i].end_time, jobs[i].status);
        }
    }
}