/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   valid_map.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 19:35:00 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/14 19:35:00 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/private/pr_parser.h"

static int	find_player(t_data *data)
{
	int		y;
	int		players;
	int		line_players;

	players = 0;
	y = 0;
	while (y < data->map_height)
	{
		line_players = inspect_map_line(data, y);
		if (line_players < 0)
			return (0);
		players += line_players;
		y++;
	}
	return (players);
}

int	valid_map(t_data *data)
{
	int	x;
	int	y;

	if (!data->map || data->map_height < 3 || find_player(data) != 1)
		return (0);
	y = 0;
	while (y < data->map_height)
	{
		x = 0;
		while (x < data->map_width)
		{
			if (is_walkable(map_at(data, x, y))
				&& !is_closed_cell(data, x, y))
				// ft_printf("Erro: celula caminhavel aberta no mapa\n");
				return (0);
			x++;
		}
		y++;
	}
	return (1);
}
