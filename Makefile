NAME = webserv

SRC = ./src/main.cpp

ROOT = ./

SRCS = ${addprefix ${ROOT}, ${SRC}}

OBJS = ${SRCS:.c=.o}

CC = c++

CFLAGS = -Wall -Wextra -Werror -I./includes -I/usr/includes -std=c++98

%.o:%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
		$(CC) $(CFLAGS) $(HEAD) $(SRC) -o $(NAME)

clean:
		rm -rf $(OBJS)

fclean: clean
		rm -rf $(NAME)

re:	fclean all

.PHONY: all clean re webserv