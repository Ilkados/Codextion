#ifndef SIMULATION_H
# define SIMULATION_H

# include <pthread.h>
# include "coder.h"
# include "dongle.h"

typedef struct s_simulation
{
	int				nb_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				nb_compiles_required;
	int				dongle_cooldown;
	int				scheduler;
	int				is_running;
	long			start_time;
	pthread_mutex_t	print_mutex;
	t_coder			*coders;
	t_dongle		*dongles;
}	t_simulation;

int		is_sim_running(t_simulation *sim);
void	stop_sim(t_simulation *sim);
void	stop_and_wake_all(t_simulation *sim);
int		init_simulation(t_simulation *sim);
int		start_threads(t_simulation *sim, pthread_t *threads,
			pthread_t *monitor);
void	join_threads(pthread_t *threads, int count);

#endif
