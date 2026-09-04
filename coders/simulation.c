/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moboulir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 23:04:49 by moboulir          #+#    #+#             */
/*   Updated: 2026/06/20 23:04:49 by moboulir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "simulation.h"

int	is_sim_running(t_simulation *sim)
{
	int	running;

	pthread_mutex_lock(&sim->print_mutex);
	running = sim->is_running;
	pthread_mutex_unlock(&sim->print_mutex);
	return (running);
}

void	stop_sim(t_simulation *sim)
{
	pthread_mutex_lock(&sim->print_mutex);
	sim->is_running = 0;
	pthread_mutex_unlock(&sim->print_mutex);
}

void	stop_and_wake_all(t_simulation *sim)
{
	stop_sim(sim);
	pthread_mutex_lock(&sim->resource_mutex);
	pthread_cond_broadcast(&sim->resource_cond);
	pthread_mutex_unlock(&sim->resource_mutex);
}
