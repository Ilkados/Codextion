/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moboulir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 23:04:50 by moboulir          #+#    #+#             */
/*   Updated: 2026/06/20 23:04:50 by moboulir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DONGLE_H
# define DONGLE_H

# include <pthread.h>
# include "queue.h"

struct	s_coder;
struct	s_simulation;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				is_taken;
	long			release_time;
	t_queue			queue;
}	t_dongle;

int		take_dongle(t_dongle *dongle, struct s_coder *coder);
void	release_dongle(t_dongle *dongle, struct s_simulation *sim);
long	compute_priority(struct s_coder *coder);

#endif
