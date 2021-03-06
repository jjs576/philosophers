/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjoo <jjoo@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/03 22:05:20 by jjoo              #+#    #+#             */
/*   Updated: 2021/06/07 18:19:19 by jjoo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	check_is_dead(t_philo *p)
{
	pthread_mutex_lock(p->checker);
	if (*p->is_dead)
		p->state = STATE_OVER;
	pthread_mutex_unlock(p->checker);
}

static void	philo_eat(t_philo *p)
{
	pthread_mutex_lock(p->fork_left);
	print_message(p, MESSAGE_FORK);
	pthread_mutex_lock(p->fork_right);
	print_message(p, MESSAGE_FORK);
	print_message(p, MESSAGE_EAT);
	p->last_eat = get_time();
	ft_sleep(ft_min(p->time_to_eat, p->time_to_die));
	p->eat_count++;
	pthread_mutex_unlock(p->fork_left);
	pthread_mutex_unlock(p->fork_right);
	p->state &= ~STATE_EAT;
	p->state |= STATE_SLEEP;
}

static void	philo_sleep(t_philo *p)
{
	size_t	time1;
	size_t	time2;

	print_message(p, MESSAGE_SLEEP);
	time1 = ft_max(p->time_to_eat, p->time_to_sleep);
	time2 = p->time_to_die - p->time_to_eat;
	ft_sleep(ft_min(time1, time2));
	p->state &= ~STATE_SLEEP;
	p->state |= STATE_THINK;
}

static void	philo_think(t_philo *p)
{
	print_message(p, MESSAGE_THINK);
	p->state &= ~STATE_THINK;
	p->state |= STATE_EAT;
}

void		*routine(void *arg)
{
	t_philo	*p;

	p = arg;
	while (!(p->state & STATE_OVER))
	{
		check_is_dead(p);
		if (p->eat_count == p->num_of_must_eat)
			p->state = STATE_OVER;
		if (get_time() - p->last_eat >= p->time_to_die)
			p->state = STATE_DEAD | STATE_OVER;
		pthread_mutex_lock(p->checker);
		if (p->state & STATE_DEAD)
			*p->is_dead = TRUE;
		pthread_mutex_unlock(p->checker);
		if (p->state & STATE_EAT)
			philo_eat(p);
		else if (p->state & STATE_SLEEP)
			philo_sleep(p);
		else if (p->state & STATE_THINK)
			philo_think(p);
	}
	pthread_mutex_lock(p->checker);
	*p->is_end += 1;
	pthread_mutex_unlock(p->checker);
	return (0);
}
