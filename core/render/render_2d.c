/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_2d.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 19:17:29 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/16 19:25:13 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game/screen.h"

static void	draw_tile(t_game *g, int x, int y, int color)
{
	int	sx;
	int	sy;
	int	size;

	size = S_TILE / 4;
	sy = 0;
	while (sy < size)
	{
		sx = 0;
		while (sx < size)
		{
			render_put_pixel(&g->frame, x * size + sx, y * size + sy, color);
			sx++;
		}
		sy++;
	}
}

void	render_2d(t_game *g)
{
	int	x;
	int	ty;
	int	color;

	ty = 0;
	while (ty < g->scene.map_height)
	{
		x = 0;
		while (x < g->scene.map_width)
		{
			if (is_walkable(map_at(&g->scene, x, ty)))
				color = COLOR_FLOOR;
			else if (map_at(&g->scene, x, ty) == ' ')
				color = COLOR_VOID;
			else
				color = COLOR_WALL;
			draw_tile(g, x, ty, color);
			x++;
		}
		ty++;
	}
	render_put_pixel(&g->frame, (int)(g->player.pos_x * S_TILE / 4),
		(int)(g->player.pos_y * S_TILE / 4), COLOR_PLAYER);
}
