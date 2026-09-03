/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moboulir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 23:04:47 by moboulir          #+#    #+#             */
/*   Updated: 2026/06/20 23:04:47 by moboulir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simulation.h"
#include "logger.h"
#include <unistd.h>
#include "coder.h"

void	smart_sleep(long sleep_time_in_ms, t_simulation *sim)
{
	long	start_time;

	start_time = get_time();
	while (is_sim_running(sim))
	{
		if ((get_time() - start_time) >= sleep_time_in_ms)
			break ;
		usleep(500);
	}
}

void	do_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->mutex);
	coder->last_compile_time = get_time();
	pthread_mutex_unlock(&coder->mutex);
	log_action(coder->sim, coder->coder_id, COMPILING);
	smart_sleep(coder->sim->time_to_compile, coder->sim);
	pthread_mutex_lock(&coder->mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->mutex);
}

void	do_debug(t_coder *coder)
{
	log_action(coder->sim, coder->coder_id, DEBUGGING);
	smart_sleep(coder->sim->time_to_debug, coder->sim);
}

void	do_refactor(t_coder *coder)
{
	log_action(coder->sim, coder->coder_id, REFACTORING);
	smart_sleep(coder->sim->time_to_refactor, coder->sim);
}

int	take_both_dongles(t_dongle *first, t_dongle *second, t_coder *coder)
{
	int	result;

	while (is_sim_running(coder->sim))
	{
		if (take_dongle(first, coder))
			return (1);
		log_action(coder->sim, coder->coder_id, TOOK_DONGLE);
		result = try_take_dongle(second, coder);
		if (result == TAKEN)
		{
			log_action(coder->sim, coder->coder_id, TOOK_DONGLE);
			return (0);
		}
		release_dongle(first);
		if (result == STOPPED)
			return (1);
	}
	return (1);
}
