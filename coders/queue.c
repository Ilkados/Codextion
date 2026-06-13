#include "queue.h"

void	enqueue(t_queue *queue, int coder_id, long priority)
{
	t_entry	entry;

	entry.coder_id = coder_id;
	entry.priority = priority;
	entry.seq = queue->seq;
	queue->seq++;
	queue->entries[queue->size] = entry;
	queue->size++;
	shift_up(queue->entries, queue->size - 1);
}

t_entry	dequeue(t_queue *queue)
{
	t_entry	entry;

	entry = queue->entries[0];
	queue->entries[0] = queue->entries[queue->size - 1];
	queue->size--;
	shift_down(queue->entries, 0, queue->size);
	return (entry);
}
