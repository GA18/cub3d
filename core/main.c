/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 16:46:02 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/07 16:12:07 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/public/pu_parser.h"

int	main(int ac, char **av)
{
	t_data_	*data;

	if (ac != 2)
		return (0);
	if (!parser_controller(av, data))
		return (0);
}
