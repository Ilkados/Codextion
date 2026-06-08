#include <pthread.h>
#include "coder.h"
#include "dongle.h"
#include "simulation.h"
#include "parsing.h"
#include "monitor.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int    main(int argc, char **argv)
{
	t_simulation    sim;
	pthread_t       *threads;
	pthread_t       monitor_thread;
	int             i;

	if (parsing(argc, argv, &sim) == 1)
	{
		printf("error\n");
		return (1);
	}
	sim.coders = malloc(sizeof(t_coder) * sim.nb_coders);
	sim.dongles = malloc(sizeof(t_dongle) * sim.nb_coders);
	threads = malloc(sizeof(pthread_t) * sim.nb_coders);
    sim.start_time = get_time();
	i = 0;
	while (i < sim.nb_coders)
	{
		//dongle info 
		sim.dongles[i].is_taken = 0;
		sim.dongles[i].release_time = 0;
		pthread_mutex_init(&sim.dongles[i].mutex, NULL);
		pthread_cond_init(&sim.dongles[i].cond, NULL);
		sim.dongles[i].queue.size = 0;
		sim.dongles[i].queue.entries = malloc(sizeof(t_entry) * 2);
		
		//coder info 
        sim.coders[i].coder_id = i + 1;
		sim.coders[i].left_dongle = &sim.dongles[i];
		sim.coders[i].right_dongle = &sim.dongles[(i + 1) % sim.nb_coders];
		sim.coders[i].last_compile_time = sim.start_time;
		sim.coders[i].compile_count = 0;
		sim.coders[i].sim = &sim;
		pthread_mutex_init(&sim.coders[i].mutex, NULL);
		i++;
	}
	sim.is_running = 1;
	pthread_mutex_init(&sim.print_mutex, NULL);
	
	i = 0;
	while (i < sim.nb_coders)
	{
		pthread_create(&threads[i], NULL, coder_routine, &sim.coders[i]);
		i++;
	}
	pthread_create(&monitor_thread, NULL, monitor_routine, &sim);
	i = 0;
	while (i < sim.nb_coders)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	pthread_join(monitor_thread, NULL);

    i =0;
    while (i < sim.nb_coders)
	{	
        
		free(sim.dongles[i].queue.entries);
        pthread_mutex_destroy(&sim.dongles[i].mutex);
        pthread_cond_destroy(&sim.dongles[i].cond);
        pthread_mutex_destroy(&sim.coders[i].mutex);
        i++;
    }
    pthread_mutex_destroy(&sim.print_mutex);
    free(sim.coders);
    free(sim.dongles);
    free(threads);
	return (0);
}