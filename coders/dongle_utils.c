#include <pthread.h>
#include <time.h>
#include "coder.h"
#include "dongle.h"
#include "queue.h"
#include "simulation.h"
#include "logger.h"


int	try_take_dongle(t_dongle *dongle, t_coder *coder)
{
	pthread_mutex_lock(&dongle->mutex);
	enqueue(&dongle->queue, coder->coder_id, compute_priority(coder));
	if (!is_sim_running(coder->sim))
	{
		remove_coder_from_queue(&dongle->queue, coder->coder_id);
		pthread_mutex_unlock(&dongle->mutex);
		return (STOPPED);
	}
	if (dongle_unavailable(dongle, coder))
	{
		remove_coder_from_queue(&dongle->queue, coder->coder_id);
		pthread_mutex_unlock(&dongle->mutex);
		return (BUSY);
	}
	dongle->is_taken = 1;
	dequeue(&dongle->queue);
	pthread_mutex_unlock(&dongle->mutex);
	return (TAKEN);
}
