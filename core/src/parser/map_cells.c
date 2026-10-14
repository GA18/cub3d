/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_cells.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 19:35:00 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/14 19:35:00 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/private/pr_parser.h"

int	is_player(char c)
{
	return (c == 'N' || c == 'S' || c == 'E' || c == 'W');
}

int	is_walkable(char c)
{
	return (c == '0' || is_player(c));
}

char	map_at(t_data *data, int x, int y)
{
	if (x < 0 || y < 0 || y >= data->map_height || x >= data->map_width)
		return (' ');
	if (x >= (int)ft_strlen(data->map[y]))
		return (' ');
	return (data->map[y][x]);
}

int	is_closed_cell(t_data *data, int x, int y)
{
	return ((map_at(data, x - 1, y) == '1'
			|| is_walkable(map_at(data, x - 1, y)))
		&& (map_at(data, x + 1, y) == '1'
			|| is_walkable(map_at(data, x + 1, y)))
		&& (map_at(data, x, y - 1) == '1'
			|| is_walkable(map_at(data, x, y - 1)))
		&& (map_at(data, x, y + 1) == '1'
			|| is_walkable(map_at(data, x, y + 1))));
}

int	inspect_map_line(t_data *data, int y)
{
	int		x;
	int		players;
	char	cell;

	players = 0;
	x = 0;
	while (x < (int)ft_strlen(data->map[y]))
	{
		cell = data->map[y][x];
		if (cell != '0' && cell != '1' && cell != ' ' && !is_player(cell))
			return (-1);
		if (is_player(cell))
		{
			players++;
			data->player_x = x;
			data->player_y = y;
			data->player_dir = cell;
		}
		x++;
	}
	return (players);
}
