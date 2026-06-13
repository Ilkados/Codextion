#include "queue.h"

void	swap(t_entry *arr, int index_a, int index_b)
{
	t_entry	temp;

	temp = arr[index_a];
	arr[index_a] = arr[index_b];
	arr[index_b] = temp;
}

int	entry_less(t_entry *a, t_entry *b)
{
	if (a->priority != b->priority)
		return (a->priority < b->priority);
	return (a->seq < b->seq);
}

void	shift_up(t_entry *arr, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (entry_less(&arr[index], &arr[parent]))
		{
			swap(arr, parent, index);
			index = parent;
		}
		else
			break ;
	}
}

void	shift_down(t_entry *arr, int index, int size)
{
	int	left;
	int	right;
	int	smallest;

	while (index < size)
	{
		left = (index * 2) + 1;
		right = (index * 2) + 2;
		smallest = index;
		if (left < size && entry_less(&arr[left], &arr[smallest]))
			smallest = left;
		if (right < size && entry_less(&arr[right], &arr[smallest]))
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
