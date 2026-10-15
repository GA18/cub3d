/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_archive.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/30 15:07:45 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/14 19:35:00 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <unistd.h>

#include "../../../includes/private/pr_parser.h"

static char	*map_line(char *line)
{
	size_t	len;

	len = ft_strlen(line);
	if (len > 0 && line[len - 1] == '\n')
		line[len - 1] = '\0';
	return (line);
}

static int	append_map_line(t_data *data, char *line)
{
	char	**new_map;
	int		index;

	new_map = realloc(data->map, sizeof(char *) * (data->map_height + 2));
	if (!new_map)
		return (0);
	data->map = new_map;
	data->map[data->map_height++] = line;
	data->map[data->map_height] = NULL;
	index = (int)ft_strlen(line);
	if (index > data->map_width)
		data->map_width = index;
	return (1);
}

static int	is_map_line(char *line, int in_map)
{
	return (in_map || (line && *line
			&& (ft_isdigit(*line) || *line == ' ')));
}

static int	process_line(char *line, t_data *data, int *in_map)
{
	char	*trimmed;

	trimmed = trim_line(ft_strdup(line));
	*in_map = is_map_line(trimmed, *in_map);
	if (*in_map)
	{
		free(trimmed);
		if (!append_map_line(data, map_line(line)))
			return (free(line), 0);
		return (1);
	}
	if (trimmed && *trimmed && !parse_config(trimmed, data))
		return (free(trimmed), free(line), 0);
	free(trimmed);
	free(line);
	return (1);
}

int	split_archive(const char *path, t_data *data)
{
	int		fd;
	int		in_map;
	char	*line;

	if (!path || ft_strlen(path) < 5
		|| ft_strncmp(path + ft_strlen(path) - 4, ".cub", 5))
		// ft_printf("Erro: caminho ou extensao do arquivo invalida\n");
		return (0);
	fd = open(path, O_RDONLY);
	if (fd < 0)
		// ft_printf("Erro: nao foi possivel abrir o arquivo\n");
		return (0);
	in_map = 0;
	line = get_next_line(fd);
	while (line)
	{
		if (!process_line(line, data, &in_map))
			return (close(fd), 0);
		line = get_next_line(fd);
	}
	close(fd);
	return (data->map_height > 0);
}
