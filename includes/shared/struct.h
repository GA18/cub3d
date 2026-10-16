#ifndef STRUCT_H
# define STRUCT_H

# include "../private/pr_parser.h"
# include "enum.h"

typedef t_data t_scene;

typedef struct s_img
{
	int		bpp;
	int		llen;
	int		endian;
	int		width;
	int		height;
	char	*addr;
	void	*ptr;
} t_img;

typedef struct s_player
{
	double	pos_x;
	double	pos_y;
	double	dir_x;
	double	dir_y;
	double	plane_x;
	double	plane_y;
} t_player;

typedef struct s_ray_hit
{
	int		hit;
	int		map_x;
	int		map_y;
	double	x;
	double	y;
	double	distance;
	t_ray_side side;
} t_ray_hit;

typedef struct s_ray_state
{
	int		map_x;
	int		map_y;
	int		step_x;
	int		step_y;
	double	delta_x;
	double	delta_y;
	double	side_x;
	double	side_y;
	t_ray_side side;
} t_ray_state;

typedef struct s_raycast
{
	double		ray_x;
	double		ray_y;
	t_ray_hit	hit;
	t_ray_state state;
} t_raycast;

typedef struct s_game
{
	void		*mlx;
	void		*win;
	t_scene		scene;
	t_img		frame;
	t_player	player;
	t_view		view;
	double		last_frame;
	int			keys[GK_COUNT];
} t_game;

#endif
