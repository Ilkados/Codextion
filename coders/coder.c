#include <unistd.h>
#include "simulation.h"
#include "coder.h"
#include "dongle.h"
#include "logger.h"

static int	take_both_dongles(t_dongle *first, t_dongle *second,
		t_coder *coder)
{
	if (take_dongle(first, coder))
		return (1);
	log_action(coder->sim, coder->coder_id, TOOK_DONGLE);
	if (take_dongle(second, coder))
	{
		release_dongle(first);
		return (1);
	}
	log_action(coder->sim, coder->coder_id, TOOK_DONGLE);
	return (0);
}

static void	do_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->mutex);
	coder->last_compile_time = get_time();
	pthread_mutex_unlock(&coder->mutex);
	log_action(coder->sim, coder->coder_id, COMPILING);
	usleep(coder->sim->time_to_compile * 1000);
	pthread_mutex_lock(&coder->mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->mutex);
}

static void	do_debug(t_coder *coder)
{
	log_action(coder->sim, coder->coder_id, DEBUGGING);
	usleep(coder->sim->time_to_debug * 1000);
}

static void	do_refactor(t_coder *coder)
{
	log_action(coder->sim, coder->coder_id, REFACTORING);
	usleep(coder->sim->time_to_refactor * 1000);
}

void	*coder_routine(void *arg)
{
	t_coder		*coder;
	t_dongle	*first;
	t_dongle	*second;

	coder = (t_coder *)arg;
	first = coder->left_dongle;
	second = coder->right_dongle;
	if (coder->coder_id % 2 == 0)
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	while (is_sim_running(coder->sim))
	{
		if (take_both_dongles(first, second, coder))
			break ;
		do_compile(coder);
		release_dongle(first);
		release_dongle(second);
		if (!is_sim_running(coder->sim))
			break ;
		do_debug(coder);
		do_refactor(coder);
	}
	return (NULL);
}
