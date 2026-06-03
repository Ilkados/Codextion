#include <pthread.h>
#include "coder.h"
#include "dongle.h"
#include "queue.h"
#include "simulation.h"
#include "logger.h"

void    take_dongle(t_dongle *dongle, t_coder *coder)
{
    long    priority;

    pthread_mutex_lock(&dongle->mutex);
    if (coder->sim->scheduler == 0)
        priority = get_time();
    else
        priority = coder->last_compile_time + coder->sim->time_to_burnout;
    enqueue(&dongle->queue, coder->coder_id, priority);
    while (dongle->is_taken == 1
        || get_time() - dongle->release_time < coder->sim->dongle_cooldown
        || dongle->queue.entries[0].coder_id != coder->coder_id)
    {
        pthread_cond_wait(&dongle->cond, &dongle->mutex);
    }
    dongle->is_taken = 1;
    dequeue(&dongle->queue);
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