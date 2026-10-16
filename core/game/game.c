/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 19:17:29 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/16 19:17:32 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/struct.h"
#include "game/common.h"
#include "game/screen.h"
#include "game/player.h"

static void _game_capframe(double *elapsed)
{
	if (*elapsed < 0.0)
		*elapsed = 0;
	if (*elapsed > 0.05)
		*elapsed = 0.05;
}

int	game_frame(t_game *g)
{
	double	now;
	double	elapsed;

	now = common_now();
	elapsed = now - g->last_frame;
	g->last_frame = now;
	_game_capframe(&elapsed);
	player_update(g, elapsed);
	screen_clear(g);
	screen_render(g);
	return (0);
}
