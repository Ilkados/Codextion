#include <unistd.h>
#include "coder.h"
#include "simulation.h"
#include "logger.h"
#include "monitor.h"

static int	check_burnout(t_coder *coder)
{
	long	time_since_compile;

	pthread_mutex_lock(&coder->mutex);
	time_since_compile = get_time() - coder->last_compile_time;
	pthread_mutex_unlock(&coder->mutex);
	if (time_since_compile > coder->sim->time_to_burnout)
		return (1);
	return (0);
}

static int	all_compiles_done(t_simulation *sim)
{
	int	i;
	int	safe_count;

	i = 0;
	while (i < sim->nb_coders)
	{
		pthread_mutex_lock(&sim->coders[i].mutex);
		safe_count = sim->coders[i].compile_count;
		pthread_mutex_unlock(&sim->coders[i].mutex);
		if (safe_count < sim->nb_compiles_required)
			return (0);
		i++;
	}
	return (1);
}

static int	find_burnout(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		if (check_burnout(&sim->coders[i]) == 1)
			return (i);
		i++;
	}
	return (-1);
}

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;
	int				i;

	sim = (t_simulation *)arg;
	while (is_sim_running(sim))
	{
		if (all_compiles_done(sim) == 1)
		{
			stop_and_wake_all(sim);
			return (NULL);
		}
		i = find_burnout(sim);
		if (i >= 0)
		{
			log_action(sim, sim->coders[i].coder_id, BURNED_OUT);
			stop_and_wake_all(sim);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
