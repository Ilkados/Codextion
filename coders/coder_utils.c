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

int	execute_cycle(t_coder *coder, t_dongle *left, t_dongle *righ)
{
	if (take_both_dongles(left, righ, coder))
		return (1);
	do_compile(coder);
	release_dongle(left);
	release_dongle(righ);
	if (!is_sim_running(coder->sim))
		return (1);
	do_debug(coder);
	if (!is_sim_running(coder->sim))
		return (1);
	do_refactor(coder);
	return (0);
}
