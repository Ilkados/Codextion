#include "simulation.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int    is_valid_number(char *str)
{
	int i;

	i = 0;
	while (str[i] != '\0')
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int    parsing(int argc, char **argv, t_simulation *sim)
{
	int i;

	if (argc != 9)
		return (1);
	i = 1;
	while (i <= 7)
	{
		if (is_valid_number(argv[i]) == 0)
			return (1);
		i++;
	}
	if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
		return (1);
	sim->nb_coders            = atoi(argv[1]);
	sim->time_to_burnout      = atoi(argv[2]);
	sim->time_to_compile      = atoi(argv[3]);
	sim->time_to_debug        = atoi(argv[4]);
	sim->time_to_refactor     = atoi(argv[5]);
	sim->nb_compiles_required = atoi(argv[6]);
	sim->dongle_cooldown      = atoi(argv[7]);
	if (strcmp(argv[8], "fifo") == 0)
		sim->scheduler = 0;
	else
		sim->scheduler = 1;
	if(sim->nb_coders <= 0)
		return (1);
	return (0);
}