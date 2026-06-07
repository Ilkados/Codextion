#include "simulation.h"
#include "coder.h"
#include "dongle.h"
#include "logger.h"
#include <unistd.h>

void    *coder_routine(void *arg)
{
	t_coder *coder;
    t_dongle *first,*second;
	coder = (t_coder *)arg;
    if(coder->coder_id % 2 == 0)
    {
        first = coder->right_dongle;
        second = coder->left_dongle;
    }
    else
    {
        first = coder->left_dongle;
        second = coder->right_dongle;
    }
	while (coder->sim->is_running)
	{
		take_dongle(first, coder);
		log_action(coder->sim, coder->coder_id, TOOK_DONGLE);
		
		take_dongle(second, coder);
		log_action(coder->sim, coder->coder_id, TOOK_DONGLE);
		coder->last_compile_time = get_time();
		log_action(coder->sim, coder->coder_id, COMPILING);
		usleep(coder->sim->time_to_compile * 1000);
		
		coder->compile_count++;
		release_dongle(first);
		release_dongle(second);
		log_action(coder->sim, coder->coder_id, DEBUGGING);
		usleep(coder->sim->time_to_debug * 1000);

		log_action(coder->sim, coder->coder_id, REFACTORING);
		usleep(coder->sim->time_to_refactor * 1000);
	}
	return (NULL);
}