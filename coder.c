#include"simulation.h"
#include"coder.h"
#include"dongle.h"
#include"logger.h"
#include<unistd.h>
void *coder_routine(void *arg)
{
    t_coder *coder = (t_coder *)arg;

    while (coder->sim->is_running)
    {
        take_dongle(coder->left_dongle,coder);
        log_action(coder->sim,coder->coder_id,TOOK_DONGLE);
        
        take_dongle(coder->right_dongle,coder);
        log_action(coder->sim,coder->coder_id,TOOK_DONGLE);
        
        coder->last_compile_time = get_time();
        
        log_action(coder->sim,coder->coder_id,COMPILING);
        usleep(coder->sim->time_to_compile * 1000);
        
        coder->compile_count++;
        
        release_dongle(coder->left_dongle);
        release_dongle(coder->right_dongle);
        
        log_action(coder->sim,coder->coder_id,DEBUGGING);
        usleep(coder->sim->time_to_debug * 1000);
        
        log_action(coder->sim,coder->coder_id,REFACTORING);
        usleep(coder->sim->time_to_refactor * 1000);
   
    }
    return (NULL);
    
}