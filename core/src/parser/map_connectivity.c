/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_connectivity.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 17:35:00 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/15 17:35:00 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/private/pr_parser.h"

static int	add_point(t_flood *flood, int x, int y)
{
	if (!is_walkable(map_at(flood->data, x, y))
		|| flood->visited[y][x])
		return (0);
	flood->visited[y][x] = 1;
	flood->queue[flood->tail++] = (t_point){x, y};
	return (1);
}

static void	add_neighbours(t_flood *flood, t_point point)
{
	add_point(flood, point.x - 1, point.y);
	add_point(flood, point.x + 1, point.y);
	add_point(flood, point.x, point.y - 1);
	add_point(flood, point.x, point.y + 1);
}

static int	init_flood(t_data *data, t_flood *flood, int total)
{
	int	y;

	flood->data = data;
	flood->visited = allocate_visited(data);
	if (!flood->visited)
		return (-1);
	flood->queue = malloc(sizeof(t_point) * total);
	if (!flood->queue)
	{
		y = data->map_height;
		while (y > 0)
			free(flood->visited[--y]);
		free(flood->visited);
		return (-1);
	}
	flood->tail = 1;
	flood->visited[data->player_y][data->player_x] = 1;
	flood->queue[0] = (t_point){data->player_x, data->player_y};
	return (0);
}

static int	run_flood(t_flood *flood)
{
	t_point	point;
	int		head;

	head = 0;
	while (head < flood->tail)
	{
		point = flood->queue[head++];
		add_neighbours(flood, point);
	}
	return (flood->tail);
}

int	validate_map_connectivity(t_data *data)
{
	t_flood	flood;
	int		total;
	int		reachable;
	int		y;

	total = count_walkable_cells(data);
	if (init_flood(data, &flood, total) < 0)
		return (-1);
	reachable = run_flood(&flood);
	free(flood.queue);
	y = data->map_height;
	while (y > 0)
		free(flood.visited[--y]);
	free(flood.visited);
	return (reachable == total);
}
