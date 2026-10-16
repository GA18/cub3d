/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_common.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 19:17:29 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/16 19:24:50 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game/screen.h"

void	render_put_pixel(t_img *image, int x, int y, int color)
{
	char	*pixel;

	if (x < 0 || x >= image->width || y < 0 || y >= image->height)
		return ;
	pixel = image->addr + y * image->llen + x * (image->bpp / 8);
	*(unsigned int *)pixel = (unsigned int)color;
}
