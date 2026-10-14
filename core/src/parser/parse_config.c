/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_config.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 19:35:00 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/14 19:35:00 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/private/pr_parser.h"

char	*trim_line(char *line)
{
	char	*trimmed;

	trimmed = ft_strtrim(line, " \t\n");
	free(line);
	return (trimmed);
}

static char	**texture_destination(char *line, t_data *data)
{
	if (!ft_strncmp(line, "NO ", 3) || !ft_strncmp(line, "NO\t", 3))
		return (&data->north_texture);
	if (!ft_strncmp(line, "SO ", 3) || !ft_strncmp(line, "SO\t", 3))
		return (&data->south_texture);
	if (!ft_strncmp(line, "WE ", 3) || !ft_strncmp(line, "WE\t", 3))
		return (&data->west_texture);
	if (!ft_strncmp(line, "EA ", 3) || !ft_strncmp(line, "EA\t", 3))
		return (&data->east_texture);
	return (NULL);
}

static int	parse_texture_config(char *line, t_data *data)
{
	char	**destination;
	char	*value;

	destination = texture_destination(line, data);
	if (!destination || *destination)
		return (0);
	value = trim_line(ft_substr(line, 3, ft_strlen(line) - 3));
	if (!value || !*value)
		return (free(value), 0);
	*destination = value;
	return (1);
}

static int	parse_color_config(char *line, t_data *data)
{
	char	*value;
	t_rgb	*color;
	int		*is_set;

	color = &data->floor;
	is_set = &data->floor_set;
	if (!ft_strncmp(line, "C ", 2) || !ft_strncmp(line, "C\t", 2))
	{
		color = &data->ceiling;
		is_set = &data->ceiling_set;
	}
	else if (ft_strncmp(line, "F ", 2) && ft_strncmp(line, "F\t", 2))
		return (0);
	if (*is_set)
		return (0);
	value = trim_line(ft_substr(line, 2, ft_strlen(line) - 2));
	if (!value || !parse_color(value, color))
		return (free(value), 0);
	free(value);
	*is_set = 1;
	return (1);
}

int	parse_config(char *line, t_data *data)
{
	if (texture_destination(line, data))
		return (parse_texture_config(line, data));
	if (!ft_strncmp(line, "F ", 2) || !ft_strncmp(line, "F\t", 2)
		|| !ft_strncmp(line, "C ", 2) || !ft_strncmp(line, "C\t", 2))
		return (parse_color_config(line, data));
	return (0);
}
