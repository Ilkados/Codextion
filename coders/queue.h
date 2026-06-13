#ifndef QUEUE_H
# define QUEUE_H

typedef struct s_entry
{
	int		coder_id;
	long	priority;
	long	seq;
}	t_entry;

typedef struct s_queue
{
	t_entry	*entries;
	int		size;
	long	seq;
}	t_queue;

void	swap(t_entry *arr, int index_a, int index_b);
int		entry_less(t_entry *a, t_entry *b);
void	shift_up(t_entry *arr, int index);
void	shift_down(t_entry *arr, int index, int size);
void	enqueue(t_queue *queue, int coder_id, long priority);
t_entry	dequeue(t_queue *queue);

#endif
