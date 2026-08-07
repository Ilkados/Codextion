/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moboulir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 23:04:48 by moboulir          #+#    #+#             */
/*   Updated: 2026/06/20 23:04:48 by moboulir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "simulation.h"
#include "logger.h"

static void	cleanup_dongles(t_simulation *sim, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		free(sim->dongles[i].queue.entries);
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		i++;
	}
}

static int	init_one_dongle(t_dongle *dongle)
{
	dongle->is_taken = 0;
	dongle->release_time = 0;
	dongle->queue.size = 0;
	dongle->queue.seq = 0;
	dongle->queue.entries = malloc(sizeof(t_entry) * 2);
	if (!dongle->queue.entries)
		return (1);
	if (pthread_mutex_init(&dongle->mutex, NULL) != 0)
	{
		free(dongle->queue.entries);
		return (1);
	}
	if (pthread_cond_init(&dongle->cond, NULL) != 0)
	{
		pthread_mutex_destroy(&dongle->mutex);
		free(dongle->queue.entries);
		return (1);
	}
	return (0);
}

static int	init_dongles(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		if (init_one_dongle(&sim->dongles[i]))
		{
			cleanup_dongles(sim, i);
			return (1);
		}
		i++;
	}
	return (0);
}

static int	init_coders(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->nb_coders)
	{
		sim->coders[i].coder_id = i + 1;
		sim->coders[i].left_dongle = &sim->dongles[i];
		sim->coders[i].right_dongle = &sim->dongles[(i + 1) % sim->nb_coders];
		sim->coders[i].last_compile_time = sim->start_time;
		sim->coders[i].compile_count = 0;
		sim->coders[i].sim = sim;
		if (pthread_mutex_init(&sim->coders[i].mutex, NULL) != 0)
		{
			while (--i >= 0)
				pthread_mutex_destroy(&sim->coders[i].mutex);
			return (1);
		}
		i++;
	}
	return (0);
}

int	init_simulation(t_simulation *sim)
{
	sim->coders = malloc(sizeof(t_coder) * sim->nb_coders);
	sim->dongles = malloc(sizeof(t_dongle) * sim->nb_coders);
	if (!sim->coders || !sim->dongles)
	{
		free(sim->coders);
		free(sim->dongles);
		return (1);
	}
	sim->start_time = get_time();
	sim->is_running = 1;
	pthread_mutex_init(&sim->print_mutex, NULL);
	if (init_dongles(sim))
	{
		free(sim->coders);
		free(sim->dongles);
		return (1);
	}
	if (init_coders(sim))
	{
		cleanup_dongles(sim, sim->nb_coders);
		free(sim->coders);
		free(sim->dongles);
		return (1);
	}
	return (0);
}
