/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moboulir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 23:01:16 by moboulir          #+#    #+#             */
/*   Updated: 2026/06/20 23:04:47 by moboulir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "simulation.h"
#include "coder.h"
#include "dongle.h"
#include "logger.h"
#include "stdio.h"

int	take_both_dongles(t_dongle *first, t_dongle *second,
				t_coder *coder)
{
	if (take_dongle(first, coder))
		return (1);
	log_action(coder->sim, coder->coder_id, TOOK_DONGLE);
	if (take_dongle(second, coder))
	{
		release_dongle(first);
		return (1);
	}
	log_action(coder->sim, coder->coder_id, TOOK_DONGLE);
	return (0);
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

void	*coder_routine(void *arg)
{
	t_coder		*coder;
	t_dongle	*left;
	t_dongle	*righ;

	coder = (t_coder *)arg;
	left = coder->left_dongle;
	righ = coder->right_dongle;
	if (coder->coder_id % 2 == 0)
	{
		left = coder->right_dongle;
		righ = coder->left_dongle;
	}
	while (is_sim_running(coder->sim))
	{
		if (execute_cycle(coder, left, righ))
			break ;
	}
	return (NULL);
}
