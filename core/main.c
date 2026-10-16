/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 16:46:02 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/16 19:11:23 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game/runtime.h"

int	main(int ac, char **av)
{
	t_game	game;

	if (runtime_args(ac, av))
		return (1);
	if (runtime_setup(&game, av[1]))
		return (1);
	return (runtime_run(&game));
}
