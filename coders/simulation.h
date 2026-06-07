#ifndef SIMULATION_H
# define SIMULATION_H

# include <pthread.h>
# include "coder.h"
# include "dongle.h"

typedef struct s_simulation
{
	int             nb_coders;
	int             time_to_burnout;
	int             time_to_compile;
	int             time_to_debug;
	int             time_to_refactor;
	int             nb_compiles_required;
	int             dongle_cooldown;
	int             scheduler;
	long            start_time;
	int             is_running;
	pthread_mutex_t print_mutex;
	t_coder         *coders;
	t_dongle        *dongles;
}   t_simulation;

#endif