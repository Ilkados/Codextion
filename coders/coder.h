/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moboulir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 23:04:49 by moboulir          #+#    #+#             */
/*   Updated: 2026/06/20 23:04:50 by moboulir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* coder.h                                            :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: moboulir <marvin@42.fr>                    +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2026/06/20 23:04:49 by moboulir          #+#    #+#             */
/* Updated: 2026/06/20 23:04:50 by moboulir         ###   ########.fr       */
/* */
/* ************************************************************************** */

#ifndef CODER_H
# define CODER_H

# include <pthread.h>
# include "dongle.h"

typedef struct s_simulation	t_simulation;

typedef struct s_coder
{
	int				coder_id;
	int				compile_count;
	long			last_compile_time;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	pthread_mutex_t	mutex;
	t_simulation	*sim;
}	t_coder;

void	*coder_routine(void *arg);
void	smart_sleep(long sleep_time_in_ms, t_simulation *sim);
int		execute_cycle(t_coder *coder, t_dongle *left, t_dongle *righ);
int		take_both_dongles(t_dongle *first, t_dongle *second, t_coder *coder);
int		can_take_both(t_dongle *first, t_dongle *second, t_coder *coder);
long	pair_cooldown_wait(t_dongle *first, t_dongle *second, t_coder *coder);
void	do_compile(t_coder *coder);
void	do_debug(t_coder *coder);
void	do_refactor(t_coder *coder);

#endif
