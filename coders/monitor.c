#include "coder.h"
#include "simulation.h"
#include "logger.h"
#include <unistd.h>

int    check_burnout(t_coder *coder)
{
	long    time_since_compile;

	time_since_compile = get_time() - coder->last_compile_time;
	if (time_since_compile > coder->sim->time_to_burnout)
		return (1);
	return (0);
}

void    *monitor_routine(void *arg)
{
	t_simulation    *sim;
	int             i;

	sim = (t_simulation *)arg;
	while (sim->is_running)
	{
		i = 0;
		while (i < sim->nb_coders)
		{
			if (check_burnout(&sim->coders[i]) == 1)
			{
				log_action(sim, sim->coders[i].coder_id, BURNED_OUT);
				sim->is_running = 0;
				return (NULL);
			}
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}