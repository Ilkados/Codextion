#ifndef LOGGER_H
# define LOGGER_H

# include "simulation.h"

# define TOOK_DONGLE   0
# define COMPILING     1
# define DEBUGGING     2
# define REFACTORING   3
# define BURNED_OUT    4

void    log_action(t_simulation *sim, int coder_id, int message_type);
long    get_time(void);

#endif


