/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hook.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 19:17:48 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/16 19:26:31 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/const.h"
#include "shared/enum.h"
#include "shared/struct.h"
#include "game/runtime.h"
#include "game/common.h"

int	hook_press(int keycode, t_game *g)
{
	int	kid;

	if (keycode == K_ESC)
		return (runtime_close(g));
	kid = common_kid(keycode);
	if (kid >= 0 && kid < GK_COUNT)
		g->keys[kid] = 1;
	return (0);
}

int	hook_release(int keycode, t_game *g)
{
	int	kid;

	kid = common_kid(keycode);
	if (kid >= 0 && kid < GK_COUNT)
		g->keys[kid] = 0;
	return (0);
}

int	hook_out(t_game *g)
{
	int	i;

	i = 0;
	while (i < GK_COUNT)
		g->keys[i++] = 0;
	return (0);
}
