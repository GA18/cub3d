/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 19:17:29 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/16 19:26:32 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <sys/time.h>
#include "shared/enum.h"
#include "shared/const.h"

int	common_kid(int keycode)
{
	if (keycode == K_A)
		return (GK_LEFT);
	if (keycode == K_D)
		return (GK_RIGHT);
	if (keycode == K_LEFT)
		return (GK_ACLOCKWISE);
	if (keycode == K_RIGHT)
		return (GK_CLOCKWISE);
	if (keycode == K_M)
		return (GK_VIEW);
	if (keycode == K_W || keycode == K_UP)
		return (GK_FORWARD);
	if (keycode == K_S || keycode == K_DOWN)
		return (GK_BACKWARD);
	return (-1);
}

double	common_now(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec + time.tv_usec / 1e6);
}
