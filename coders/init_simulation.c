/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_simulation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moboulir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 00:00:00 by moboulir          #+#    #+#             */
/*   Updated: 2026/09/04 00:00:00 by moboulir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "simulation.h"
#include "logger.h"

static void	free_simulation_arrays(t_simulation *sim)
{
	free(sim->coders);
	free(sim->dongles);
}

static int	init_synchronization(t_simulation *sim)
{
	if (pthread_mutex_init(&sim->print_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&sim->resource_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->print_mutex);
		return (1);
	}
	if (pthread_cond_init(&sim->resource_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->resource_mutex);
		pthread_mutex_destroy(&sim->print_mutex);
		return (1);
	}
	return (0);
}

static void	cleanup_synchronization(t_simulation *sim)
{
	pthread_cond_destroy(&sim->resource_cond);
	pthread_mutex_destroy(&sim->resource_mutex);
	pthread_mutex_destroy(&sim->print_mutex);
}

static void	cleanup_dongles(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		free(sim->dongles[i].queue.entries);
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		i++;
	}
}

int	init_simulation(t_simulation *sim)
{
	sim->coders = malloc(sizeof(t_coder) * sim->nb_coders);
	sim->dongles = malloc(sizeof(t_dongle) * sim->nb_coders);
	if (!sim->coders || !sim->dongles || init_synchronization(sim))
		return (free_simulation_arrays(sim), 1);
	sim->start_time = get_time();
	sim->is_running = 1;
	if (init_dongles(sim))
		return (cleanup_synchronization(sim), free_simulation_arrays(sim), 1);
	if (init_coders(sim))
	{
		cleanup_dongles(sim);
		cleanup_synchronization(sim);
		return (free_simulation_arrays(sim), 1);
	}
	return (0);
}
