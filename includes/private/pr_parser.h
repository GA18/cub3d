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

typedef struct s_map_
{
	char	**map_;
}	t_map_;

typedef struct s_rgb_
{
	char			**flag_;
	unsigned int	rgb_;
}	t_rgb_;

typedef struct s_test
{
	int	r:8;
	int	g:8;
	int	b:8;
}	t_test;

typedef struct s_path_
{
	char	**identifier_;
	char	**path_;
}	t_path_;

typedef struct s_data_
{
	struct s_path_	*path_;
	struct s_rgb_	*rgb;
	struct s_map_	*map;
}	t_data_;

int	split_archive(char **av, t_data_ *data);
int	valid_texture(t_data_ *data);
int	valid_map(t_data_ *data);

#endif
