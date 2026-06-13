#include <stdlib.h>
#include "simulation.h"
#include "logger.h"

static int	init_dongles(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		sim->dongles[i].is_taken = 0;
		sim->dongles[i].release_time = 0;
		sim->dongles[i].queue.size = 0;
		sim->dongles[i].queue.seq = 0;
		pthread_mutex_init(&sim->dongles[i].mutex, NULL);
		pthread_cond_init(&sim->dongles[i].cond, NULL);
		sim->dongles[i].queue.entries
			= malloc(sizeof(t_entry) * (sim->nb_coders + 1));
		if (!sim->dongles[i].queue.entries)
		{
			while (--i >= 0)
				free(sim->dongles[i].queue.entries);
			return (1);
		}
		i++;
	}
	return (0);
}

static void	init_coders(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		sim->coders[i].coder_id = i + 1;
		sim->coders[i].left_dongle = &sim->dongles[i];
		sim->coders[i].right_dongle = &sim->dongles[(i + 1) % sim->nb_coders];
		sim->coders[i].last_compile_time = sim->start_time;
		sim->coders[i].compile_count = 0;
		sim->coders[i].sim = sim;
		pthread_mutex_init(&sim->coders[i].mutex, NULL);
		i++;
	}
}

int	init_simulation(t_simulation *sim)
{
	sim->coders = malloc(sizeof(t_coder) * sim->nb_coders);
	sim->dongles = malloc(sizeof(t_dongle) * sim->nb_coders);
	if (!sim->coders || !sim->dongles)
	{
		free(sim->coders);
		free(sim->dongles);
		return (1);
	}
	sim->start_time = get_time();
	sim->is_running = 1;
	pthread_mutex_init(&sim->print_mutex, NULL);
	if (init_dongles(sim))
	{
		free(sim->coders);
		free(sim->dongles);
		return (1);
	}
	init_coders(sim);
	return (0);
}
