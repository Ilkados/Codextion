# include <sys/time.h>
# include <stdio.h>
# include <pthread.h>
# include "logger.h"

long    get_time(void)
{
    struct timeval  tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void    log_action(t_simulation *sim, int coder_id, int message_type)
{
    char    *messages[] = {
        "has taken a dongle",
        "is compiling",
        "is debugging",
        "is refactoring",
        "burned out"
    };
    long    timestamp;

    pthread_mutex_lock(&sim->print_mutex);
    timestamp = get_time() - sim->start_time;
    printf("%ld %d %s\n", timestamp, coder_id, messages[message_type]);
    pthread_mutex_unlock(&sim->print_mutex);
}