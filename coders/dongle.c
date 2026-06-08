#include <pthread.h>
#include "coder.h"
#include "dongle.h"
#include "queue.h"
#include "simulation.h"
#include "logger.h"
#include <time.h>

void take_dongle(t_dongle *dongle, t_coder *coder)
{
    long priority;
    long safe_compile_time;
    pthread_mutex_lock(&dongle->mutex);
    if (coder->sim->scheduler == 0)
        priority = get_time();
    else
    {
        pthread_mutex_lock(&coder->mutex);
        safe_compile_time = coder->last_compile_time;
        pthread_mutex_unlock(&coder->mutex);
        priority = safe_compile_time + coder->sim->time_to_burnout;
    }
    
    enqueue(&dongle->queue, coder->coder_id, priority);
    while (coder->sim->is_running
        && (dongle->is_taken == 1
        || get_time() - dongle->release_time < coder->sim->dongle_cooldown
        || dongle->queue.entries[0].coder_id != coder->coder_id))
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_nsec += 10000000;
        if (ts.tv_nsec >= 1000000000)
        {
            ts.tv_sec++;
            ts.tv_nsec -= 1000000000;
        }
        pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
    }
    if (coder->sim->is_running)
    {
        dongle->is_taken = 1;
        dequeue(&dongle->queue);
    }
    pthread_mutex_unlock(&dongle->mutex);
}

void    release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_taken = 0;
	dongle->release_time = get_time();
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}