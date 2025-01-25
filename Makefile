NAME    = ircserv
BOT_NAME	= bot

CC      = c++
CFLAGS  = -Wall -Wextra -Werror -std=c++98
DEBUG_FLAGS = -g -DDEBUG
RM      = rm -rf

SRC_DIR = src/
BOT_DIR     = src/bot/
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

BOT_SRC     =   Bot \
                Player \
                Logger \
                Utils \

SRCS    = $(addprefix $(SRC_DIR), $(addsuffix .cpp, $(SRC)))
BOT_SRCS    = $(addprefix $(BOT_DIR), $(addsuffix .cpp, $(BOT_SRC)))

OBJS    = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(SRC)))
BOT_OBJS    = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(BOT_SRC)))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/server
	mkdir -p $(OBJ_DIR)/bot

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)%.o: $(BOT_DIR)%.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# Rules for the bot
bot: $(BOT_NAME)

$(BOT_NAME): $(BOT_OBJS)
	$(CC) $(CFLAGS) -o $(BOT_NAME) $(BOT_OBJS)

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME) $(BOT_NAME)

re: fclean all

debug: CFLAGS += $(DEBUG_FLAGS)
debug: re

.PHONY: all clean fclean re debug bot
