/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 19:17:29 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/16 19:26:32 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shared/cub3d.h"
#include "game/runtime.h"
#include "game/hook.h"
#include "game/game.h"
#include "game/common.h"

static void	setup_player(t_game *g)
{
	g->player.pos_x = g->scene.player_x + 0.5;
	g->player.pos_y = g->scene.player_y + 0.5;
	g->player.dir_x = 0.0;
	g->player.dir_y = 0.0;
	g->player.plane_x = 0.0;
	g->player.plane_y = 0.0;
	if (g->scene.player_dir == 'N')
		g->player.dir_y = -1.0;
	else if (g->scene.player_dir == 'S')
		g->player.dir_y = 1.0;
	else if (g->scene.player_dir == 'E')
		g->player.dir_x = 1.0;
	else
		g->player.dir_x = -1.0;
	if (g->player.dir_x != 0.0)
		g->player.plane_y = 0.66 * g->player.dir_x;
	else
		g->player.plane_x = -0.66 * g->player.dir_y;
	g->view = V_3D;
	g->last_frame = common_now();
}

int	setup_mlx(t_game *g)
{
	g->mlx = mlx_init();
	if (!g->mlx)
		return (1);
	g->win = mlx_new_window(g->mlx, S_WIDTH, S_HEIGHT, G_NAME);
	if (!g->win)
		return (1);
	return (0);
}

int	setup_hook(t_game *g)
{
	mlx_hook(g->win, EVT_KEY_PRESS, 0, hook_press, g);
	mlx_hook(g->win, EVT_KEY_RELEASE, 0, hook_release, g);
	mlx_hook(g->win, EVT_FOCUS_OUT, 0, hook_out, g);
	mlx_hook(g->win, EVT_EXIT, 0, runtime_close, g);
	mlx_loop_hook(g->mlx, game_frame, g);
	return (0);
}

int	setup_frame(t_game *g)
{
	g->frame.width = S_WIDTH;
	g->frame.height = S_HEIGHT;
	g->frame.ptr = mlx_new_image(g->mlx, g->frame.width, g->frame.height);
	if (!g->frame.ptr)
		return (1);
	g->frame.addr = mlx_get_data_addr(g->frame.ptr, &g->frame.bpp,
			&g->frame.llen, &g->frame.endian);
	if (!g->frame.addr)
		return (1);
	ft_memset(g->frame.addr, 0, g->frame.llen * g->frame.height);
	return (0);
}

int	setup_game(t_game *g)
{
	setup_player(g);
	if (setup_mlx(g))
		return (runtime_cleanup(g), 1);
	if (setup_hook(g))
		return (runtime_cleanup(g), 1);
	if (setup_frame(g))
		return (runtime_cleanup(g), 1);
	return (0);
}
