#include <string.h>
#include <stdlib.h>
#include "simulation.h"
#include "parsing.h"

static int	is_valid_number(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '\0')
		return (0);
	if (str[i] == '-')
		return (0);
	if (str[i] == '+')
		i++;
	if (str[i] == '\0')
		return (0);
	while (str[i] != '\0')
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int	fits_in_int(char *str)
{
	int	i;
	int	len;

	i = 0;
	if (str[i] == '+')
		i++;
	while (str[i] == '0' && str[i + 1] != '\0')
		i++;
	len = (int)strlen(str + i);
	if (len > 10)
		return (0);
	if (len == 10 && strcmp(str + i, "2147483647") > 0)
		return (0);
	return (1);
}

static void	set_values(char **argv, t_simulation *sim)
{
	sim->nb_coders = atoi(argv[1]);
	sim->time_to_burnout = atoi(argv[2]);
	sim->time_to_compile = atoi(argv[3]);
	sim->time_to_debug = atoi(argv[4]);
	sim->time_to_refactor = atoi(argv[5]);
	sim->nb_compiles_required = atoi(argv[6]);
	sim->dongle_cooldown = atoi(argv[7]);
	sim->scheduler = 1;
	if (strcmp(argv[8], "fifo") == 0)
		sim->scheduler = 0;
}

int	parsing(int argc, char **argv, t_simulation *sim)
{
	int	i;

	if (argc != 9)
		return (1);
	i = 1;
	while (i <= 7)
	{
		if (!is_valid_number(argv[i]) || !fits_in_int(argv[i]))
			return (1);
		i++;
	}
	if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
		return (1);
	set_values(argv, sim);
	if (sim->nb_coders <= 0)
		return (1);
	return (0);
}
