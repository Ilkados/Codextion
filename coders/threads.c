#include <pthread.h>
#include "simulation.h"
#include "coder.h"
#include "monitor.h"

void	join_threads(pthread_t *threads, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
}

int	start_threads(t_simulation *sim, pthread_t *threads, pthread_t *monitor)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		if (pthread_create(&threads[i], NULL,
				coder_routine, &sim->coders[i]) != 0)
		{
			stop_and_wake_all(sim);
			join_threads(threads, i);
			return (1);
		}
		i++;
	}
	if (pthread_create(monitor, NULL, monitor_routine, sim) != 0)
	{
		stop_and_wake_all(sim);
		join_threads(threads, sim->nb_coders);
		return (1);
	}
	return (0);
}
