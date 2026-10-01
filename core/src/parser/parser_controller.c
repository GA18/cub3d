/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_controller.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: g-alves- <g-alves-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 18:59:40 by g-alves-          #+#    #+#             */
/*   Updated: 2026/10/01 16:30:35 by g-alves-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/private/pr_parser.h"

int	parser_controller(char **av, t_data_ *data)
{
	if (!split_archive(av, data))
		return (0);
	if (!valid_texture(data))
		return (0);
	if (!valid_map(data))
		return (0);
}
