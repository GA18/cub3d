/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 16:46:02 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/15 16:51:49 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/public/pu_parser.h"

int	main(int ac, char **av)
{
	t_data	data;

	if (ac != 2)
		return (1);
	ft_bzero(&data, sizeof(data));
	if (!parser_controller(av[1], &data))
	{
		ft_printf("Mapa inválido!");
		return (free_data(&data), 1);
	}
	free_data(&data);
	ft_printf("mapa válido!");
	return (0);
}
