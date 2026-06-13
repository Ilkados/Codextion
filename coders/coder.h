#ifndef CODER_H
# define CODER_H

# include <pthread.h>
# include "dongle.h"

struct	s_simulation;

typedef struct s_coder
{
	int						coder_id;
	int						compile_count;
	long					last_compile_time;
	pthread_mutex_t			mutex;
	t_dongle				*left_dongle;
	t_dongle				*right_dongle;
	struct s_simulation		*sim;
}	t_coder;

void	*coder_routine(void *arg);

#endif
