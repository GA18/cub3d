/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   screen.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 19:17:29 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/16 19:26:32 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game/screen.h"
#include "libft.h"
#include "shared/enum.h"

int	screen_clear(t_game *g)
{
	if (!g || !g->frame.addr)
		return (1);
	ft_memset(g->frame.addr, 0, g->frame.llen * g->frame.height);
	return (0);
}

int	screen_render(t_game *g)
{
	if (g->view == V_3D)
		render_3d(g);
	else
		render_2d(g);
	mlx_put_image_to_window(g->mlx, g->win, g->frame.ptr, 0, 0);
	return (0);
}
