/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_pair.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moboulir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 00:00:00 by moboulir          #+#    #+#             */
/*   Updated: 2026/09/04 00:00:00 by moboulir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simulation.h"
#include "logger.h"
#include "queue.h"

static void	register_for_both(t_dongle *first, t_dongle *second,
		t_coder *coder, long priority)
{
	enqueue(&first->queue, coder->coder_id, priority);
	enqueue(&second->queue, coder->coder_id, priority);
}

static void	take_pair(t_dongle *first, t_dongle *second)
{
	first->is_taken = 1;
	second->is_taken = 1;
	dequeue(&first->queue);
	dequeue(&second->queue);
}

static void	wait_for_pair(t_dongle *first, t_dongle *second,
		t_coder *coder)
{
	long	wait_time;

	while (is_sim_running(coder->sim)
		&& !can_take_both(first, second, coder))
	{
		wait_time = pair_cooldown_wait(first, second, coder);
		if (wait_time > 0)
		{
			pthread_mutex_unlock(&coder->sim->resource_mutex);
			smart_sleep(wait_time, coder->sim);
			pthread_mutex_lock(&coder->sim->resource_mutex);
		}
		else
			pthread_cond_wait(&coder->sim->resource_cond,
				&coder->sim->resource_mutex);
	}
}

int	take_both_dongles(t_dongle *first, t_dongle *second, t_coder *coder)
{
	long	priority;

	if (first == second)
		return (1);
	priority = compute_priority(coder);
	pthread_mutex_lock(&coder->sim->resource_mutex);
	register_for_both(first, second, coder, priority);
	wait_for_pair(first, second, coder);
	if (!is_sim_running(coder->sim))
	{
		pthread_mutex_unlock(&coder->sim->resource_mutex);
		return (1);
	}
	take_pair(first, second);
	pthread_mutex_unlock(&coder->sim->resource_mutex);
	log_action(coder->sim, coder->coder_id, TOOK_DONGLE);
	log_action(coder->sim, coder->coder_id, TOOK_DONGLE);
	return (0);
}
