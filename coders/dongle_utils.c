#include <pthread.h>
#include <time.h>
#include "coder.h"
#include "dongle.h"
#include "queue.h"
#include "simulation.h"
#include "logger.h"

static int	second_is_still_busy(t_dongle *dongle, t_coder *coder)
{
	if (dongle->is_taken)
		return (1);
	if (dongle->queue.size > 0)
		return (1);
	if (get_time() - dongle->release_time
		< coder->sim->dongle_cooldown)
		return (1);
	return (0);
}

static void	set_cooldown_timeout(t_dongle *dongle,
	t_coder *coder, struct timespec *timeout)
{
	long	wake_time;

	wake_time = dongle->release_time + coder->sim->dongle_cooldown;
	timeout->tv_sec = wake_time / 1000;
	timeout->tv_nsec = (wake_time % 1000) * 1000000;
}

void	wait_for_second_dongle_change(t_dongle *dongle, t_coder *coder)
{
	struct timespec	timeout;

	pthread_mutex_lock(&dongle->mutex);
	while (is_sim_running(coder->sim)
		&& second_is_still_busy(dongle, coder))
	{
		if (!dongle->is_taken && dongle->queue.size == 0)
		{
			set_cooldown_timeout(dongle, coder, &timeout);
			pthread_cond_timedwait(&dongle->cond,
				&dongle->mutex, &timeout);
		}
		else
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	pthread_mutex_unlock(&dongle->mutex);
}
int	try_take_dongle(t_dongle *dongle, t_coder *coder)
{
	pthread_mutex_lock(&dongle->mutex);
	enqueue(&dongle->queue, coder->coder_id, compute_priority(coder));
	if (dongle_unavailable(dongle, coder))
	{
		remove_coder_from_queue(&dongle->queue, coder->coder_id);
		pthread_mutex_unlock(&dongle->mutex);
		return (BUSY);
	}
	if (!is_sim_running(coder->sim))
	{
		remove_coder_from_queue(&dongle->queue, coder->coder_id);
		pthread_mutex_unlock(&dongle->mutex);
		return (STOPPED);
	}
	dongle->is_taken = 1;
	dequeue(&dongle->queue);
	pthread_mutex_unlock(&dongle->mutex);
	return (TAKEN);
}
