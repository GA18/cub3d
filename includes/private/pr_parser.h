/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pr_parser.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 16:47:50 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/07 16:34:17 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PR_PARSER_H
# define PR_PARSER_H

# include "../../libft/libft.h"

typedef struct s_rgb
{
	int	r;
	int	g;
	int	b;
}	t_rgb;

typedef struct s_point
{
	int	x;
	int	y;
}	t_point;

typedef struct s_data
{
	char	*north_texture;
	char	*south_texture;
	char	*west_texture;
	char	*east_texture;
	t_rgb	floor;
	t_rgb	ceiling;
	int		floor_set;
	int		ceiling_set;
	char	**map;
	int		map_height;
	int		map_width;
	int		player_x;
	int		player_y;
	char	player_dir;
}	t_data;

typedef struct s_flood
{
	t_data	*data;
	char	**visited;
	t_point	*queue;
	int		tail;
}	t_flood;

int		parser_controller(const char *path, t_data *data);
int		split_archive(const char *path, t_data *data);
char	*trim_line(char *line);
int		parse_color(char *value, t_rgb *color);
int		parse_config(char *line, t_data *data);
int		is_player(char c);
int		is_walkable(char c);
char	map_at(t_data *data, int x, int y);
int		is_closed_cell(t_data *data, int x, int y);
int		inspect_map_line(t_data *data, int y);
int		valid_texture(t_data *data);
int		valid_map(t_data *data);
int		count_walkable_cells(t_data *data);
char	**allocate_visited(t_data *data);
int		validate_map_connectivity(t_data *data);
void	free_data(t_data *data);

#endif
