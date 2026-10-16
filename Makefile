NAME = cub3d
PARSER_NAME = parser_check

CC = cc
CFLAGS = -Wall -Wextra -Werror
CPPFLAGS = -Iincludes -Ilibft -Ilibrary/mlx
LDLIBS = -Llibft -lft
MLX_LDLIBS = -Llibrary/mlx -lmlx -lXext -lX11 -lm
RM = rm -f

LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a
MLX_DIR = library/mlx
MLX = $(MLX_DIR)/libmlx.a

PARSER_SRCS = core/src/parser/parser_controller.c \
	core/src/parser/map_cells.c \
	core/src/parser/map_connectivity.c \
	core/src/parser/map_connectivity_utils.c \
	core/src/parser/parse_color.c \
	core/src/parser/parse_config.c \
	core/src/parser/read_archive.c \
	core/src/parser/valid_routes.c \
	core/src/parser/valid_map.c
PARSER_OBJS = $(PARSER_SRCS:.c=.o)

GAME_SRCS = core/main.c core/runtime.c core/setup.c core/common.c \
	core/game/game.c core/input/hook.c core/player/player.c \
	core/render/raycast.c core/render/render_common.c \
	core/render/render_2d.c core/render/render_3d.c core/render/screen.c
GAME_OBJS = $(GAME_SRCS:.c=.o)
PARSER_MAIN_OBJ = core/parser_main.o

all: $(NAME) $(PARSER_NAME)

$(NAME): $(LIBFT) $(MLX) $(PARSER_OBJS) $(GAME_OBJS)
	$(CC) $(CFLAGS) $(GAME_OBJS) $(PARSER_OBJS) $(LDLIBS) $(MLX_LDLIBS) -o $@

$(PARSER_NAME): $(LIBFT) $(PARSER_OBJS) $(PARSER_MAIN_OBJ)
	$(CC) $(CFLAGS) $(PARSER_MAIN_OBJ) $(PARSER_OBJS) $(LDLIBS) -o $@

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(MLX):
	$(MAKE) -C $(MLX_DIR)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	$(RM) $(PARSER_OBJS) $(GAME_OBJS) $(PARSER_MAIN_OBJ)
	$(MAKE) -C $(LIBFT_DIR) clean
	@if [ -f $(MLX_DIR)/makefile.gen ]; then $(MAKE) -C $(MLX_DIR) clean; fi

fclean: clean
	$(RM) $(NAME) $(PARSER_NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re
