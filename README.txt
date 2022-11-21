Homework4

Objective:

To implement a simple job scheduler that executes non-interactive jobs (e.g., jobs that do not
have direct user interaction, jobs that can run in the background).

Description:
In this project we will run the jobs in the background using threads and queues. We have one main thread and one child chread the child thread will be responsible to run the jobs.

Compilation Steps:
1. Either enter make or gcc -Wall -o homework4 queue.c homework4.c -lpthread
2. This will create a homework4 executing file.

Executing Steps:
1. Run the program using ./homework4 <max_no_of_jobs_to_run_at_the_same_time>
2. Once you execute you will see the shell '>'
3. enter submit <job_command> to run the given job
4. enter showjobs to show the jobs that are currently running or waiting in the background.
5. enter submithistory for all the history of jobs that are ran successfully.