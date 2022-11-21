/* Header file for the simple circular queue example */
#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct _job{
    int job_id;
    char *args[1000];
	char start_time[1000];
	char end_time[1000];
    char input_command[1000];
    char *status;
} job;

typedef struct _queue {
	int size;    /* maximum size of the queue */
	job **buffer; /* queue buffer */
	int start;   /* index to the start of the queue */
	int end;     /* index to the end of the queue */
	int count;   /* no. of elements in the queue */
} queue;

queue *queue_init(int n);
job *queue_delete(queue *q);
int queue_insert(queue *q, job *item);
// void queue_display(queue *q);
void queue_destroy(queue *q);

#endif