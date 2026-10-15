/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_color.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 19:35:00 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/14 19:35:00 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/private/pr_parser.h"

static int	parse_rgb_value(char *value, int *result)
{
	char	*end;
	int		number;

	while (*value == ' ' || *value == '\t')
		value++;
	if (!ft_isdigit(*value))
		// ft_printf("Erro: componente de cor nao numerico\n");
		return (0);
	number = 0;
	while (ft_isdigit(*value))
	{
		number = number * 10 + (*value - '0');
		if (number > 255)
			// ft_printf("Erro: componente de cor fora do intervalo 0-255\n");
			return (0);
		value++;
	}
	end = value;
	while (*end == ' ' || *end == '\t')
		end++;
	if (*end != '\0')
		// ft_printf("Erro: caracteres extras no componente de cor\n");
		return (0);
	*result = number;
	return (1);
}

static void	free_parts(char **parts)
{
	int	i;

	i = 0;
	while (parts && parts[i])
		free(parts[i++]);
	free(parts);
}

int	parse_color(char *value, t_rgb *color)
{
	char	**parts;
	int		ok;

	parts = ft_split(value, ',');
	if (!parts || !parts[0] || !parts[1] || !parts[2] || parts[3])
		// ft_printf("Erro: formato de cor invalido\n");
		return (free_parts(parts), 0);
	ok = parse_rgb_value(parts[0], &color->r);
	if (ok)
		ok = parse_rgb_value(parts[1], &color->g);
	if (ok)
		ok = parse_rgb_value(parts[2], &color->b);
	free_parts(parts);
	return (ok);
}
