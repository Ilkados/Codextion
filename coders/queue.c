#include "queue.h"

void    swap(t_entry *arr, int index_a, int index_b)
{
	t_entry temp;

	temp         = arr[index_a];
	arr[index_a] = arr[index_b];
	arr[index_b] = temp;
}

void    sift_up(t_entry *arr, int index)
{
	int parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (arr[index].priority < arr[parent].priority)
		{
			swap(arr, parent, index);
			index = parent;
		}
		else
			break ;
	}
}

void    sift_down(t_entry *arr, int index, int size)
{
	int left;
	int right;
	int smallest;

	while (index < size)
	{
		left     = (index * 2) + 1;
		right    = (index * 2) + 2;
		smallest = index;
		if (left < size && arr[left].priority < arr[smallest].priority)
			smallest = left;
		if (right < size && arr[right].priority < arr[smallest].priority)
			smallest = right;
		if (smallest != index)
		{
			swap(arr, smallest, index);
			index = smallest;
		}
		else
			break ;
	}
}

void    enqueue(t_queue *queue, int coder_id, long priority)
{
	t_entry coder;

	coder.coder_id = coder_id;
	coder.priority = priority;
	queue->entries[queue->size] = coder;
	queue->size++;
	sift_up(queue->entries, queue->size - 1);
}

t_entry dequeue(t_queue *queue)
{
	t_entry coder;

	coder = queue->entries[0];
	queue->entries[0] = queue->entries[queue->size - 1];
	queue->size--;
	sift_down(queue->entries, 0, queue->size);
	return (coder);
}