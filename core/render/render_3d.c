/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_3d.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 19:17:29 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/16 19:27:20 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game/raycast.h"
#include "game/screen.h"

static void	fill_background(t_game *g)
{
	int	x;
	int	ty;

	ty = 0;
	while (ty < S_HEIGHT)
	{
		x = 0;
		while (x < S_WIDTH)
		{
			if (ty < S_HEIGHT / 2)
				render_put_pixel(&g->frame, x, ty, COLOR_VOID);
			else
				render_put_pixel(&g->frame, x, ty, COLOR_FLOOR);
			x++;
		}
		ty++;
	}
}

static void	render_wall_column(t_game *g, int x)
{
	int		y;
	int		top;
	int		bottom;
	int		wall_height;

	g->raycast.ray_x = g->player.dir_x + g->player.plane_x
		* (2.0 * x / (double)S_WIDTH - 1.0);
	g->raycast.ray_y = g->player.dir_y + g->player.plane_y
		* (2.0 * x / (double)S_WIDTH - 1.0);
	if (raycast_cast(g, &g->raycast) && g->raycast.hit.distance > 0.0)
	{
		wall_height = (int)(S_HEIGHT / g->raycast.hit.distance);
		top = (S_HEIGHT - wall_height) / 2;
		bottom = top + wall_height;
		y = top;
		while (y < bottom)
			render_put_pixel(&g->frame, x, y++, COLOR_WALL_3D);
	}
}

void	render_3d(t_game *g)
{
	int	x;

	fill_background(g);
	x = 0;
	while (x < S_WIDTH)
		render_wall_column(g, x++);
}
