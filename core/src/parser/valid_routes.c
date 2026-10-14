/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   valid_routes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 15:08:31 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/14 19:35:00 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/private/pr_parser.h"

int	valid_texture(t_data *data)
{
	return (data->north_texture && data->south_texture
		&& data->west_texture && data->east_texture
		&& data->floor_set && data->ceiling_set);
}

void	free_data(t_data *data)
{
	int	i;

	if (!data)
		return ;
	free(data->north_texture);
	free(data->south_texture);
	free(data->west_texture);
	free(data->east_texture);
	i = 0;
	while (data->map && data->map[i])
		free(data->map[i++]);
	free(data->map);
	ft_bzero(data, sizeof(*data));
}
