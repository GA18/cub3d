/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycast.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 19:17:29 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/16 19:17:29 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RAYCAST_H
# define RAYCAST_H

# include "shared/struct.h"

void	raycast_setup(t_game *g, t_raycast *raycast);
void	raycast_advance(t_raycast *raycast);
int		raycast_in_boundaries(t_game *g, t_raycast *raycast);
int		raycast_is_hit(t_game *g, t_raycast *raycast);
int		raycast_cast(t_game *g, t_raycast *raycast);

#endif
