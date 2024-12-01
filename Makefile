NAME	= ircserv

CC		= c++
CFLAGS	= -Wall -Wextra -Werror -std=c++98
D_FLAGS = -g -DDEBUG
RM		= rm -rf

SRC_DIR = src/
OBJ_DIR = obj/
INC_DIR = inc/

SRC		=	Channel \
			Client \
			Commands \
			IRCServer \
			Utils \
			Main \
			Message

SRCS	= $(addprefix $(SRC_DIR), $(addsuffix .cpp, $(SRC)))
OBJS	= $(addprefix $(OBJ_DIR), $(addsuffix .o, $(SRC)))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

debug: CFLAGS += $(DEBUG_FLAGS)
debug: re

.PHONY: all clean fclean re debug
