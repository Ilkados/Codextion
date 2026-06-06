#ifndef CODER_H
# define CODER_H

# include <pthread.h>

typedef struct s_simulation t_simulation;
typedef struct s_dongle     t_dongle;

typedef struct s_coder
{
    int             coder_id;
    t_dongle        *left_dongle;
    t_dongle        *right_dongle;
    long            last_compile_time;
    int             compile_count;
    t_simulation    *sim;
    pthread_mutex_t mutex;
}   t_coder;

#endif