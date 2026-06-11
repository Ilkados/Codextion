#include <pthread.h>
#include "coder.h"
#include "dongle.h"
#include "queue.h"
#include "simulation.h"
#include "logger.h"
#include <time.h>

static long	compute_priority(t_coder *coder)
{
	long	last_compile;

	if (coder->sim->scheduler == 0)
		return (get_time());
	pthread_mutex_lock(&coder->mutex);
	last_compile = coder->last_compile_time;
	pthread_mutex_unlock(&coder->mutex);
	return (last_compile + coder->sim->time_to_burnout);
}

static int	dongle_unavailable(t_dongle *dongle, t_coder *coder)
{
	if (dongle->is_taken == 1)
		return (1);
	if (dongle->queue.entries[0].coder_id != coder->coder_id)
		return (1);
	if (get_time() - dongle->release_time < coder->sim->dongle_colddown)
		return (1);
	return (0);
}

static void	cooldown_sleep(t_dongle *dongle, t_coder *coder)
{
	struct timespec	ts;
	long			target_ms;

	target_ms = dongle->release_time + coder->sim->dongle_colddown;
	ts.tv_sec = target_ms / 1000;
	ts.tv_nsec = (target_ms % 1000) * 1000000;
	pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
}

static void	wait_for_dongle(t_dongle *dongle, t_coder *coder)
{
	while (coder->sim->is_running && dongle_unavailable(dongle, coder))
	{
		if (dongle->is_taken == 0
			&& dongle->queue.entries[0].coder_id == coder->coder_id)
			cooldown_sleep(dongle, coder);
		else
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
}

void	take_dongle(t_dongle *dongle, t_coder *coder)
{
	pthread_mutex_lock(&dongle->mutex);
	enqueue(&dongle->queue, coder->coder_id, compute_priority(coder));
	wait_for_dongle(dongle, coder);
	if (coder->sim->is_running)
	{
		dongle->is_taken = 1;
		dequeue(&dongle->queue);
	}
	pthread_mutex_unlock(&dongle->mutex);
}

void	release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_taken = 0;
	dongle->release_time = get_time();
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}