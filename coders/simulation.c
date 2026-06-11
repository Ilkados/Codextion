#include "simulation.h"

int is_sim_running(t_simulation *sim)
{
	int	running;

	pthread_mutex_lock(&sim->print_mutex);
	running = sim->is_running;
	pthread_mutex_unlock(&sim->print_mutex);
	return (running);
}