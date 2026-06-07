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

int all_compiles_done(t_coder *coders,int nb_compiles_required,int total_coder )
{
	 int i=0;
	 while (i<total_coder)
	 {
		if(coders[i].compile_count < nb_compiles_required)
			return 0 ;
		i++;
	 }
	return  1;
}
void    *monitor_routine(void *arg)
{
	t_simulation    *sim;
	int             i;

	sim = (t_simulation *)arg;
	while (sim->is_running)
	{
		if( all_compiles_done (sim->coders,sim->nb_compiles_required,sim->nb_coders)== 1 )
		{
			sim->is_running = 0;
			return (NULL);
		}
		i = 0;
		int j;
		while (i < sim->nb_coders)
		{
			if (check_burnout(&sim->coders[i]) == 1)
			{
				log_action(sim, sim->coders[i].coder_id, BURNED_OUT);
				sim->is_running = 0;
				j =0;
				while (j< sim->nb_coders)
				{
					pthread_cond_broadcast(&sim->dongles->cond);
					j++;
				}
				
			}
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}