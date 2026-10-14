NAME = cub3d

CC = cc
CFLAGS = -Wall -Wextra -Werror
CPPFLAGS = -Iincludes/private -Ilibft
RM = rm -f

LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

SRCS = core/main.c \
	core/src/parser/parser_controller.c \
	core/src/parser/map_cells.c \
	core/src/parser/parse_color.c \
	core/src/parser/parse_config.c \
	core/src/parser/read_archive.c \
	core/src/parser/valid_routes.c \
	core/src/parser/valid_map.c
OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	$(RM) $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

.PHONY: all clean fclean re
