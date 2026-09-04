/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pair_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moboulir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/04 00:00:00 by moboulir          #+#    #+#             */
/*   Updated: 2026/09/04 00:00:00 by moboulir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simulation.h"
#include "logger.h"

static long	cooldown_left(t_dongle *dongle, t_coder *coder)
{
	long	left;

	left = coder->sim->dongle_cooldown
		- (get_time() - dongle->release_time);
	if (left < 0)
		return (0);
	return (left);
}

static int	is_first(t_dongle *dongle, t_coder *coder)
{
	if (dongle->queue.size == 0)
		return (0);
	return (dongle->queue.entries[0].coder_id == coder->coder_id);
}

int	can_take_both(t_dongle *first, t_dongle *second, t_coder *coder)
{
	if (!is_first(first, coder) || !is_first(second, coder))
		return (0);
	if (first->is_taken || second->is_taken)
		return (0);
	if (cooldown_left(first, coder) > 0 || cooldown_left(second, coder) > 0)
		return (0);
	return (1);
}

long	pair_cooldown_wait(t_dongle *first, t_dongle *second, t_coder *coder)
{
	long	first_wait;
	long	second_wait;

	if (!is_first(first, coder) || !is_first(second, coder))
		return (0);
	if (first->is_taken || second->is_taken)
		return (0);
	first_wait = cooldown_left(first, coder);
	second_wait = cooldown_left(second, coder);
	if (first_wait > second_wait)
		return (first_wait);
	return (second_wait);
}
