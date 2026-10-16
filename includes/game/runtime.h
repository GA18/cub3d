/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   runtime.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 19:17:29 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/16 19:17:29 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RUNTIME_H
#define RUNTIME_H
	#include "shared/cub3d.h"

	int runtime_run(t_game *g);
	int runtime_close(t_game *g);
	int runtime_cleanup(t_game *g);
	int runtime_args(int argc, char **argv);
	int runtime_setup(t_game *g, char *map_path);
#endif
