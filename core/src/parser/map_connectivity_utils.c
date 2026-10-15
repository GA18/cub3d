/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_connectivity_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 18:05:00 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/15 18:05:00 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/private/pr_parser.h"

int	count_walkable_cells(t_data *data)
{
	int	x;
	int	y;
	int	count;

	count = 0;
	y = 0;
	while (y < data->map_height)
	{
		x = 0;
		while (x < (int)ft_strlen(data->map[y]))
		{
			if (is_walkable(data->map[y][x]))
				count++;
			x++;
		}
		y++;
	}
	return (count);
}

char	**allocate_visited(t_data *data)
{
	char	**visited;
	int		y;

	visited = ft_calloc(data->map_height + 1, sizeof(char *));
	if (!visited)
		return (NULL);
	y = 0;
	while (y < data->map_height)
	{
		visited[y] = ft_calloc(ft_strlen(data->map[y]) + 1, sizeof(char));
		if (!visited[y])
		{
			while (y > 0)
				free(visited[--y]);
			free(visited);
			return (NULL);
		}
		y++;
	}
	return (visited);
}
