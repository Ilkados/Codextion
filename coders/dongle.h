#ifndef DONGLE_H
# define DONGLE_H

# include <pthread.h>
# include "queue.h"

typedef struct s_coder	t_coder;

typedef struct s_dongle
{
	int             is_taken;
	long            release_time;
	t_queue         queue;
	pthread_mutex_t mutex;
	pthread_cond_t  cond;
}   t_dongle;

void	take_dongle(t_dongle *dongle, t_coder *coder);
void	release_dongle(t_dongle *dongle);

#endif