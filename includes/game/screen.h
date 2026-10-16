/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   screen.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 19:17:29 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/16 19:17:29 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCREEN_H
#define SCREEN_H
	#include "shared/cub3d.h"

	int screen_clear(t_game *g);
	int screen_render(t_game *g);
	void render_put_pixel(t_img *image, int x, int y, int color);
	void render_2d(t_game *g);
	void render_3d(t_game *g);
#endif
