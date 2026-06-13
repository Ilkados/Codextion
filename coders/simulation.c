#include "simulation.h"

int	is_sim_running(t_simulation *sim)
{
	int	running;

	pthread_mutex_lock(&sim->print_mutex);
	running = sim->is_running;
	pthread_mutex_unlock(&sim->print_mutex);
	return (running);
}

void	stop_sim(t_simulation *sim)
{
	pthread_mutex_lock(&sim->print_mutex);
	sim->is_running = 0;
	pthread_mutex_unlock(&sim->print_mutex);
}

void	stop_and_wake_all(t_simulation *sim)
{
	int	i;

	stop_sim(sim);
	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}
