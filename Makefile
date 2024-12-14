NAME    = ircserv

CC      = c++
CFLAGS  = -Wall -Wextra -Werror -std=c++98
DEBUG_FLAGS = -g -DDEBUG
RM      = rm -rf

SRC_DIR = src/
OBJ_DIR = obj/
INC_DIR = inc/

SRC     =   Channel \
            Client \
            Utils \
            Main \
            Message \
            Logger \
			server/IRCServer \
			server/Commands \
            server/HCMessage \
            server/Registration \
			server/Disconection \
			server/ChannelCommands \
			TrivialBot \

SRCS    = $(addprefix $(SRC_DIR), $(addsuffix .cpp, $(SRC)))
OBJS    = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(SRC)))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/server

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
