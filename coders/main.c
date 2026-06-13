#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "simulation.h"
#include "parsing.h"
#include "logger.h"

static int	error_exit(char *msg)
{
	fprintf(stderr, "Error: %s\n", msg);
	return (1);
}

static void	cleanup(t_simulation *sim, pthread_t *threads)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		free(sim->dongles[i].queue.entries);
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		pthread_mutex_destroy(&sim->coders[i].mutex);
		i++;
	}
	pthread_mutex_destroy(&sim->print_mutex);
	free(sim->coders);
	free(sim->dongles);
	free(threads);
}

int	main(int argc, char **argv)
{
	t_simulation	sim;
	pthread_t		*threads;
	pthread_t		monitor_thread;

	if (parsing(argc, argv, &sim))
		return (error_exit("invalid arguments"));
	if (sim.nb_compiles_required == 0)
		return (0);
	threads = malloc(sizeof(pthread_t) * sim.nb_coders);
	if (!threads)
		return (error_exit("allocation failed"));
	if (init_simulation(&sim))
	{
		free(threads);
		return (error_exit("initialization failed"));
	}
	if (start_threads(&sim, threads, &monitor_thread))
	{
		cleanup(&sim, threads);
		return (error_exit("thread creation failed"));
	}
	join_threads(threads, sim.nb_coders);
	pthread_join(monitor_thread, NULL);
	cleanup(&sim, threads);
	return (0);
}
