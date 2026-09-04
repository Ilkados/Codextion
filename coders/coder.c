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

int	execute_cycle(t_coder *coder, t_dongle *left, t_dongle *right)
{
	if (take_both_dongles(left, right, coder))
		return (1);
	do_compile(coder);
	release_dongle(left);
	release_dongle(right);
	if (!is_sim_running(coder->sim))
		return (1);
	do_debug(coder);
	if (!is_sim_running(coder->sim))
		return (1);
	do_refactor(coder);
	return (0);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (is_sim_running(coder->sim))
	{
		if (execute_cycle(coder, coder->left_dongle, coder->right_dongle))
			break ;
	}
	return (NULL);
}
