/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moboulir <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 23:04:50 by moboulir          #+#    #+#             */
/*   Updated: 2026/06/20 23:04:50 by moboulir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_H
# define LOGGER_H

# include "simulation.h"

# define TOOK_DONGLE 0
# define COMPILING 1
# define DEBUGGING 2
# define REFACTORING 3
# define BURNED_OUT 4

long	get_time(void);
void	log_action(t_simulation *sim, int coder_id, int message_type);

#endif
