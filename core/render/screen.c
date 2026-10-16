
#include "game/screen.h"
#include "game/raycast.h"
#include "libft.h"
#include "shared/enum.h"

static void put_pixel(t_img *image, int x, int y, int color)
{
	char	*pixel;

	if (x < 0 || x >= image->width || y < 0 || y >= image->height)
		return ;
	pixel = image->addr + y * image->llen + x * (image->bpp / 8);
	*(unsigned int *)pixel = (unsigned int)color;
}

static void render_2d(t_game *g)
{
	int	x;
	int	ty;
	int	size;
	int	color;

	size = S_TILE / 4;
	ty = 0;
	while (ty < g->scene.map_height)
	{
		x = 0;
		while (x < g->scene.map_width)
		{
			color = is_walkable(map_at(&g->scene, x, ty))
				? COLOR_FLOOR : COLOR_WALL;
			if (map_at(&g->scene, x, ty) == ' ')
				color = COLOR_VOID;
			int sy = 0;
			while (sy < size)
			{
				int sx = 0;
				while (sx < size)
					put_pixel(&g->frame, x * size + sx, ty * size + sy, color), sx++;
				sy++;
			}
			x++;
		}
		ty++;
	}
	put_pixel(&g->frame, (int)(g->player.pos_x * size),
		(int)(g->player.pos_y * size), COLOR_PLAYER);
}

static void render_3d(t_game *g)
{
	int		x;
	int		y;
	int		top;
	int		bottom;
	int		wall_height;
	double	camera;
	t_raycast rc;

	for (y = 0; y < S_HEIGHT / 2; y++)
		for (x = 0; x < S_WIDTH; x++)
			put_pixel(&g->frame, x, y, COLOR_VOID);
	for (y = S_HEIGHT / 2; y < S_HEIGHT; y++)
		for (x = 0; x < S_WIDTH; x++)
			put_pixel(&g->frame, x, y, COLOR_FLOOR);
	x = 0;
	while (x < S_WIDTH)
	{
		camera = 2.0 * x / (double)S_WIDTH - 1.0;
		rc.ray_x = g->player.dir_x + g->player.plane_x * camera;
		rc.ray_y = g->player.dir_y + g->player.plane_y * camera;
		if (raycast_cast(g, &rc) && rc.hit.distance > 0.0)
		{
			wall_height = (int)(S_HEIGHT / rc.hit.distance);
			top = (S_HEIGHT - wall_height) / 2;
			bottom = top + wall_height;
			y = top;
			while (y < bottom)
				put_pixel(&g->frame, x, y++, COLOR_WALL_3D);
		}
		x++;
	}
}

int screen_clear(t_game *g)
{
	if (!g || !g->frame.addr)
		return (1);
	ft_memset(g->frame.addr, 0, g->frame.llen * g->frame.height);
	return (0);
}

int screen_render(t_game *g)
{
	if (g->view == V_3D)
		render_3d(g);
	else
		render_2d(g);
	mlx_put_image_to_window(g->mlx, g->win, g->frame.ptr, 0, 0);
	return (0);
}
