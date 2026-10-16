/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 19:17:29 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/16 19:24:50 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "public/pu_parser.h"

int	main(int ac, char **av)
{
	t_data	data;

	if (ac != 2)
		return (1);
	ft_bzero(&data, sizeof(data));
	if (!parser_controller(av[1], &data))
		return (free_data(&data), 1);
	free_data(&data);
	return (0);
}
