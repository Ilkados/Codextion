NAME = codexion

CC = cc

FLAGS = -Wall -Wextra -Werror -pthread

SRC = coders/main.c coders/logger.c coders/queue.c coders/parsing.c coders/dongle.c coders/coder.c coders/monitor.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re