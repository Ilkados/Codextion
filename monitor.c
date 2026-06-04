#include"coder.h"
#include"simulation.h"
#include"logger.h"
int  check_burnout(t_coder *coder)
{
    int last_compile = get_time() - coder->last_compile_time ;

    if(last_compile > coder->sim->time_to_burnout)
        return 1;
    return 0;
}

void monitor_routine(void *arg)
{
    t_simulation *sim = (t_simulation *)arg;
    while(sim->is_running)
    {
        int i = 0;

        while (i < sim->nb_coders)
        {
            if(check_burnout(&sim->coders[i]) ==1)
            {
                sim->is_running =0;
                log_action(sim,sim->coders[i].coder_id,BURNED_OUT);
            }
            i++;
        }
        usleep(1000);
    }
    return NULL;
}