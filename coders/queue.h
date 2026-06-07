#ifndef QUEUE_H
# define QUEUE_H

typedef struct s_entry
{
	int     coder_id;
	long    priority;
}   t_entry;

typedef struct s_queue
{
	t_entry *entries;
	int     size;
}   t_queue;

void    enqueue(t_queue *queue, int coder_id, long priority);
t_entry dequeue(t_queue *queue);

#endif